#include "ItemList.hpp"
#include "support/JavaStringTrim.hpp"
#include <algorithm>
namespace minima::json {
namespace {
void appendItem(const std::shared_ptr<ItemList::List>& list,ItemList::Item item){
    if(!list)throw JavaNullPointerException("null append list");list->add(std::move(item));
}
// Same surrogate-pair arithmetic as JavaUnicode; unpaired units stay as code points.
uint32_t nextPoint(const std::u16string& text,size_t& offset){
    uint32_t point=text[offset++];
    if(point>=0xd800&&point<=0xdbff&&offset<text.size()&&text[offset]>=0xdc00&&text[offset]<=0xdfff)
        point=0x10000+((point-0xd800)<<10)+(text[offset++]-0xdc00);
    return point;
}
std::u16string render(const ItemList::Item& item){
    if(auto text=std::get_if<std::u16string>(&item))return *text;
    return unicode::decodeUTF8(std::get<MessageObject>(item).toString());
}
}
ItemList::ItemList(const String& s){split(s,sp,items);}
ItemList::ItemList(const String& s,const String& separator):sp(s){split(s,separator,items);}
ItemList::ItemList(const String& s,const String& separator,bool multiToken){split(s,separator,items,multiToken);}
std::vector<ItemList::String> ItemList::getArray() const {throw JavaClassCastException("Object[] cannot be cast to String[]");}
void ItemList::split(const String& s,const String& separator,const std::shared_ptr<List>& append,bool multiToken){
    if(!s||!separator)return;
    if(!multiToken){split(s,separator,append);return;}
    std::vector<uint32_t> delimiters;for(size_t at=0;at<separator->size();)delimiters.push_back(nextPoint(*separator,at));
    auto isDelimiter=[&](uint32_t point){return std::find(delimiters.begin(),delimiters.end(),point)!=delimiters.end();};
    size_t pos=0;
    while(pos<s->size()){
        size_t next=pos;
        if(isDelimiter(nextPoint(*s,next))){pos=next;continue;}
        auto start=pos;pos=next;
        while(pos<s->size()){next=pos;if(isDelimiter(nextPoint(*s,next)))break;pos=next;}
        appendItem(append,javaStringTrim(s->substr(start,pos-start)));
    }
}
void ItemList::split(const String& s,const String& separator,const std::shared_ptr<List>& append){
    if(!s||!separator)return;
    size_t pos=0,previous=0;
    do{
        previous=pos;pos=s->find(*separator,pos);
        if(pos==std::u16string::npos)break;
        appendItem(append,javaStringTrim(s->substr(previous,pos-previous)));pos+=separator->size();
    }while(pos!=std::u16string::npos);
    appendItem(append,javaStringTrim(s->substr(previous)));
}
void ItemList::add(int32_t index,const String& item){if(item)items->add(size_t(int64_t(index)),javaStringTrim(*item));}
void ItemList::add(const String& item){if(item)items->add(javaStringTrim(*item));}
void ItemList::addAll(const ItemList* list){
    if(!list)throw JavaNullPointerException("null ItemList");
    std::vector<Item> copy(list->items->begin(),list->items->end());for(const auto& item:copy)items->add(item);
}
ItemList::String ItemList::get(int32_t index) const {
    if(index<0||size_t(index)>=items->size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(index)+" out of bounds for length "+std::to_string(items->size()));
    const auto& item=items->get(size_t(index));if(auto text=std::get_if<std::u16string>(&item))return *text;
    const auto& object=std::get<MessageObject>(item);if(object.isNull())return std::nullopt;
    try{return unicode::decodeUTF8(object.as<std::string>());}catch(const std::bad_any_cast&){throw JavaClassCastException("list item is not String");}
}
std::u16string ItemList::toString(const String& separator) const {
    std::u16string out;for(size_t index=0;index<items->size();++index){if(index)out+=separator.value_or(u"null");auto item=items->get(index);out+=render(item);}return out;
}
}
