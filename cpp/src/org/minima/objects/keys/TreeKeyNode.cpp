#include "TreeKeyNode.hpp"
namespace minima {
TreeKeyNode::TreeKeyNode(const MiniData& privateSeed,int size):mSize(size),mChildSeed(Crypto::getInstance().hashObject(privateSeed)) {
    if(size<0) throw std::invalid_argument("Negative key count");
    mChildren.resize(size_t(size));mKeys.reserve(size_t(size));
    for(int i=0;i<size;++i) {
        auto seed=Crypto::getInstance().hashAllObjects(MiniNumber(i),privateSeed);mKeys.emplace_back(seed);
        auto pubkey=mKeys.back().getPublicKey().value();mTree.addEntry(MMRData::CreateMMRDataLeafNode(pubkey,MiniNumber(0)));
    }
    auto root=mTree.getRoot();if(!root) throw std::invalid_argument("Empty key tree");mPublicKey=root->getData();
}
TreeKeyNode& TreeKeyNode::getChild(int child) {
    if(child<0 || size_t(child)>=mChildren.size()) throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(child)+" out of bounds for length "+std::to_string(mChildren.size()));
    auto& node=mChildren.at(size_t(child));if(!node) {auto seed=Crypto::getInstance().hashAllObjects(MiniNumber(child),mChildSeed);node=std::make_unique<TreeKeyNode>(seed,mSize);}return *node;
}
MMRProof TreeKeyNode::getProof(int key) const {return mTree.getProof(MMREntryNumber(key));}
}
