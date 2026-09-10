#pragma once
#include "Winternitz.hpp"
#include "SignatureProof.hpp"
#include "org/minima/objects/mmr/MMR.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
class TreeKeyNode {
    int mSize;
    MMR mTree;
    std::vector<std::unique_ptr<TreeKeyNode>> mChildren;
    std::vector<Winternitz> mKeys;
    MiniData mChildSeed,mPublicKey;
    std::optional<SignatureProof> mParentChildSig;
public:
    TreeKeyNode(const MiniData& privateSeed,int size);
    TreeKeyNode& getChild(int child);
    const MiniData& getPublicKey() const {return mPublicKey;}
    const Winternitz& getWOTSKey(int key) const {
        if(key<0 || size_t(key)>=mKeys.size()) throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(key)+" out of bounds for length "+std::to_string(mKeys.size()));
        return mKeys[size_t(key)];
    }
    MMRProof getProof(int key) const;
    bool childSigExists() const {return mParentChildSig.has_value();}
    void setParentChildSig(SignatureProof signature) {mParentChildSig=std::move(signature);}
    const std::optional<SignatureProof>& getParentChildSig() const {return mParentChildSig;}
};
}
