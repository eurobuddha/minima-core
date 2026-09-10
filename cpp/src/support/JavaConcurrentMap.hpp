#pragma once
#include "JavaUnicode.hpp"
#include <bit>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
namespace minima {
// String-key map adapter. Operations are synchronized and enumerations snapshot
// values in the pinned JDK's quiescent ConcurrentHashMap traversal order.
// Native red-black links serve lookup and the JDK's small-tree transition rule.
template<class T, bool HashMap = false> class JavaConcurrentMap {
    struct Node {
        std::optional<std::u16string> key;uint32_t hash=0;std::optional<T> value;
        Node *parent=nullptr,*child[2]{nullptr,nullptr};bool red=false;
    };
    struct Bin {std::list<std::shared_ptr<Node>> entries;Node* root=nullptr;bool tree=false;};
    Node mNil;
    std::vector<Bin> mBins{16};
    size_t mSize=0;
    bool mAllocated=false;
    mutable std::mutex mMutex;
    static uint32_t hash(const std::optional<std::u16string>& key) {uint32_t h=0;if(key)for(auto c:*key)h=h*31+uint16_t(c);return (h^(h>>16))&(HashMap?0xffffffffu:0x7fffffffu);}
    static bool hashLess(uint32_t a,uint32_t b) {if constexpr(HashMap)return std::bit_cast<int32_t>(a)<std::bit_cast<int32_t>(b);else return a<b;}
    void rootToFront(Bin& bin) {if constexpr(HashMap){for(auto it=bin.entries.begin();it!=bin.entries.end();++it)if(it->get()==bin.root){bin.entries.splice(bin.entries.begin(),bin.entries,it);break;}}}
    static bool less(const Node* a,const Node* b) {return hashLess(a->hash,b->hash)||(a->hash==b->hash&&a->key<b->key);}
    Node* nil() {return &mNil;}
    // Rotate a subtree toward side (0 left, 1 right), preserving parent links.
    void rotate(Bin& bin,Node* top,int side) {
        auto pivot=top->child[1-side];top->child[1-side]=pivot->child[side];
        if(pivot->child[side]!=nil())pivot->child[side]->parent=top;
        pivot->parent=top->parent;
        if(top->parent==nil())bin.root=pivot;else top->parent->child[top==top->parent->child[1]]=pivot;
        pivot->child[side]=top;top->parent=pivot;
    }
    void insertTree(Bin& bin,Node* node,bool link=false) {
        node->child[0]=node->child[1]=nil();node->red=true;Node* parent=nil();auto current=bin.root?bin.root:nil();
        while(current!=nil()){parent=current;current=current->child[!less(node,current)];}
        if constexpr(HashMap){if(link&&parent!=nil()){auto last=std::prev(bin.entries.end());for(auto it=bin.entries.begin();it!=bin.entries.end();++it)if(it->get()==parent){bin.entries.splice(std::next(it),bin.entries,last);break;}}}
        node->parent=parent;if(parent==nil())bin.root=node;else parent->child[!less(node,parent)]=node;
        while(node->parent->red) {
            auto p=node->parent,g=p->parent;int side=p==g->child[1];auto uncle=g->child[1-side];
            if(uncle->red){p->red=false;uncle->red=false;g->red=true;node=g;continue;}
            if(node==p->child[1-side]){node=p;rotate(bin,node,side);p=node->parent;g=p->parent;}
            p->red=false;g->red=true;rotate(bin,g,1-side);
        }
        bin.root->red=false;
    }
    void makeTree(Bin& bin) {bin.tree=true;bin.root=nil();for(const auto& node:bin.entries)insertTree(bin,node.get());rootToFront(bin);}
    void replace(Bin& bin,Node* old,Node* replacement) {
        if(old->parent==nil())bin.root=replacement;else old->parent->child[old==old->parent->child[1]]=replacement;
        replacement->parent=old->parent;
    }
    void eraseTree(Bin& bin,Node* node) {
        if(bin.entries.empty()||!bin.root||bin.root==nil()||bin.root->child[1]==nil()||bin.root->child[0]==nil()||bin.root->child[0]->child[0]==nil()) {bin.tree=false;bin.root=nullptr;return;}
        auto removed=node;bool red=removed->red;Node* replacement;
        if(node->child[0]==nil()){replacement=node->child[1];replace(bin,node,replacement);}
        else if(node->child[1]==nil()){replacement=node->child[0];replace(bin,node,replacement);}
        else {
            removed=node->child[1];while(removed->child[0]!=nil())removed=removed->child[0];
            red=removed->red;replacement=removed->child[1];
            if(removed->parent==node)replacement->parent=removed;
            else {replace(bin,removed,replacement);removed->child[1]=node->child[1];removed->child[1]->parent=removed;}
            replace(bin,node,removed);removed->child[0]=node->child[0];removed->child[0]->parent=removed;removed->red=node->red;
        }
        if(!red) {
            auto x=replacement;
            while(x!=bin.root&&!x->red){
                auto p=x->parent;int side=x==p->child[1];auto sibling=p->child[1-side];
                if(sibling->red){sibling->red=false;p->red=true;rotate(bin,p,side);sibling=p->child[1-side];}
                if(!sibling->child[0]->red&&!sibling->child[1]->red){if(sibling!=nil())sibling->red=true;x=p;}
                else {
                    if(!sibling->child[1-side]->red){sibling->child[side]->red=false;sibling->red=true;rotate(bin,sibling,1-side);sibling=p->child[1-side];}
                    sibling->red=p->red;p->red=false;sibling->child[1-side]->red=false;rotate(bin,p,side);x=bin.root;
                }
            }
            x->red=false;
        }
        mNil.parent=nil();rootToFront(bin);
    }
    void resize() {
        const size_t size=mBins.size();std::vector<Bin> next(size*2);
        for(size_t i=0;i<size;++i){auto& old=mBins[i];if(old.entries.empty())continue;
            if(old.tree){
                for(const auto& node:old.entries)next[i+((node->hash&size)?size:0)].entries.push_back(node);
                for(size_t index:{i,i+size}){auto& bin=next[index];if(bin.entries.size()>6){if(bin.entries.size()==old.entries.size()){bin.tree=true;bin.root=old.root;}else makeTree(bin);}}
            }else if constexpr(HashMap){
                for(const auto& node:old.entries)next[i+((node->hash&size)?size:0)].entries.push_back(node);
            }else{
                // The JDK shares the last equal-bit run and prepends the earlier
                // entries when transferring an ordinary linked bin.
                auto last=old.entries.begin();bool bit=((*last)->hash&size)!=0;
                for(auto it=std::next(last);it!=old.entries.end();++it)if(bool((*it)->hash&size)!=bit){bit=((*it)->hash&size)!=0;last=it;}
                for(auto it=last;it!=old.entries.end();++it)next[i+(((*it)->hash&size)?size:0)].entries.push_back(*it);
                for(auto it=old.entries.begin();it!=last;++it)next[i+(((*it)->hash&size)?size:0)].entries.push_front(*it);
            }
        }
        mBins=std::move(next);
    }
    void treeify(size_t index) {
        if constexpr(HashMap){if(mBins.size()<64)resize();else makeTree(mBins[index]);return;}
        if(mBins.size()<64){auto requested=std::bit_ceil(mBins.size()*3+1);while((mBins.size()*3)/4<requested)resize();}
        else makeTree(mBins[index]);
    }
    Node* find(const Bin& bin,const std::optional<std::u16string>& key,uint32_t h) const {
        if(bin.tree){auto current=bin.root;while(current&&current!=&mNil){if(current->hash==h&&current->key==key)return current;bool right=hashLess(current->hash,h)||(current->hash==h&&current->key<key);current=current->child[right];}return nullptr;}
        for(const auto& node:bin.entries)if(node->hash==h&&node->key==key)return node.get();return nullptr;
    }
public:
    JavaConcurrentMap(){mNil.parent=mNil.child[0]=mNil.child[1]=nil();}
    void reserveForPutAll(size_t incomingSize) requires HashMap {
        std::lock_guard lock(mMutex);
        if(!incomingSize)return;
        if(!mAllocated) {
            auto requested=std::min(size_t(1)<<30,incomingSize+incomingSize/3+(incomingSize%3!=0));
            mBins=std::vector<Bin>(std::bit_ceil(std::max(size_t(2),requested)));
        }else while(incomingSize>(mBins.size()*3)/4 && mBins.size()<(size_t(1)<<30))resize();
    }
    size_t size() const {std::lock_guard lock(mMutex);return mSize;}
    std::optional<T> getUTF16Key(const std::u16string& key) const{return getKey(key);}
    std::optional<T> putUTF16Key(const std::u16string& key,T value){return putKey(key,std::move(value));}
    std::optional<T> get(const std::string& text) const {return getKey(unicode::decodeUTF8(text));}
    std::optional<T> get(std::nullptr_t) const requires HashMap {return getKey(std::nullopt);}
    bool containsKey(const std::string& key) const {return get(key).has_value();}
    bool containsKey(std::nullptr_t) const requires HashMap {return get(nullptr).has_value();}
    std::optional<T> put(const std::string& text,T value) {return putKey(unicode::decodeUTF8(text),std::move(value));}
    std::optional<T> put(std::nullptr_t,T value) requires HashMap {return putKey(std::nullopt,std::move(value));}
    std::optional<T> remove(const std::string& text) {return removeKey(unicode::decodeUTF8(text));}
    std::optional<T> remove(std::nullptr_t) requires HashMap {return removeKey(std::nullopt);}
    // Object-key adapters supply their Java hashCode and a stable equality key.
    // The ordinary string API and its tree/resize behavior are unchanged.
    std::optional<T> getHashedKey(const std::string& key,uint32_t rawHash) const requires HashMap {return getKey(unicode::decodeUTF8(key),rawHash^(rawHash>>16));}
    std::optional<T> putHashedKey(const std::string& key,uint32_t rawHash,T value) requires HashMap {return putKey(unicode::decodeUTF8(key),std::move(value),rawHash^(rawHash>>16));}
    std::optional<T> removeHashedKey(const std::string& key,uint32_t rawHash) requires HashMap {return removeKey(unicode::decodeUTF8(key),rawHash^(rawHash>>16));}
private:
    std::optional<T> getKey(const std::optional<std::u16string>& key,std::optional<uint32_t> suppliedHash={}) const {auto h=suppliedHash.value_or(hash(key));std::lock_guard lock(mMutex);auto node=find(mBins[h&(mBins.size()-1)],key,h);return node?node->value:std::nullopt;}
    std::optional<T> putKey(std::optional<std::u16string> key,T value,std::optional<uint32_t> suppliedHash={}) {
        if constexpr(!HashMap && requires{value.get();}){if(!value)throw std::invalid_argument("Null ConcurrentHashMap value");}
        auto h=suppliedHash.value_or(hash(key));std::lock_guard lock(mMutex);mAllocated=true;auto index=h&(mBins.size()-1);auto& bin=mBins[index];
        if(auto node=find(bin,key,h)){auto old=node->value;node->value=std::move(value);if(!HashMap&&!bin.tree){size_t position=0;for(const auto& entry:bin.entries){++position;if(entry.get()==node)break;}if(position>=8)treeify(index);}return old;}
        auto node=std::make_shared<Node>();node->key=std::move(key);node->hash=h;node->value=std::move(value);
        auto previous=bin.entries.size();if(bin.tree){if constexpr(HashMap)bin.entries.push_back(node);else bin.entries.push_front(node);insertTree(bin,node.get(),true);rootToFront(bin);}else{bin.entries.push_back(node);if(previous>=8)treeify(index);}
        ++mSize;if(HashMap?mSize>(mBins.size()*3)/4:mSize>=(mBins.size()*3)/4)resize();return std::nullopt;
    }
    std::optional<T> removeKey(const std::optional<std::u16string>& key,std::optional<uint32_t> suppliedHash={}) {
        auto h=suppliedHash.value_or(hash(key));std::lock_guard lock(mMutex);auto& bin=mBins[h&(mBins.size()-1)];
        for(auto it=bin.entries.begin();it!=bin.entries.end();++it)if((*it)->hash==h&&(*it)->key==key){auto node=*it;bin.entries.erase(it);if(bin.tree)eraseTree(bin,node.get());--mSize;return node->value;}
        return std::nullopt;
    }
public:
    void clear(){std::lock_guard lock(mMutex);for(auto& bin:mBins){bin.entries.clear();bin.root=nullptr;bin.tree=false;}mSize=0;}
    std::vector<T> elements() const {std::lock_guard lock(mMutex);std::vector<T> result;result.reserve(mSize);for(const auto& bin:mBins)for(const auto& node:bin.entries)result.push_back(*node->value);return result;}
    std::vector<std::string> keys() const {std::lock_guard lock(mMutex);std::vector<std::string> result;result.reserve(mSize);for(const auto& bin:mBins)for(const auto& node:bin.entries)result.push_back(unicode::encodeUTF8(node->key.value()));return result;}
    std::vector<std::optional<std::string>> nullableKeys() const requires HashMap {std::lock_guard lock(mMutex);std::vector<std::optional<std::string>> result;result.reserve(mSize);for(const auto& bin:mBins)for(const auto& node:bin.entries)result.push_back(node->key?std::optional(unicode::encodeUTF8(*node->key)):std::nullopt);return result;}
};
template<class T> using JavaHashMap = JavaConcurrentMap<T,true>;
}
