#include "TreeKey.hpp"
#include <bit>
#include <cmath>
#include <limits>
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
TreeKey::TreeKey(const MiniData& seed,int keys,int levels):mLevels(levels),mKeysPerLevel(keys),mPrivateSeed(seed) {
    if(levels>MAX_KEY_LEVELS) throw std::invalid_argument("Too many key Levels "+std::to_string(levels)+" MAX:"+std::to_string(MAX_KEY_LEVELS));
    double max=std::pow(double(keys),double(levels));
    mMaxUses=std::isnan(max)?0:max>=INT32_MAX?INT32_MAX:max<=INT32_MIN?INT32_MIN:int(max);
    mRoot=std::make_unique<TreeKeyNode>(seed,keys);mPublicKey=mRoot->getPublicKey();
}
std::vector<int> TreeKey::baseConversion(int number,int base,int levels) {
    std::vector<int> nodes;
    while(number!=0) {if(base==0) throw std::invalid_argument("Zero key base");int quotient=number/base;nodes.push_back(number-quotient*base);number=quotient;}
    while(int(nodes.size())<levels) nodes.push_back(0);std::reverse(nodes.begin(),nodes.end());return nodes;
}
void TreeKey::log(const std::string& text) const {if(mLogger)mLogger(text);else MinimaLogger::log(text);}
Signature TreeKey::sign(const MiniData* data) {
    // Preserve upstream's counter reset on exhaustion. The production caller
    // must enforce its key-use policy; this is observable Java behavior.
    if(mUses>=mMaxUses){log("SERIOUS ERROR : MAX TREEKEYS USED @ "+(mPublicKey?mPublicKey->to0xString():"null"));mUses=0;}
    auto nodes=baseConversion(mUses,mKeysPerLevel,mLevels);Signature signature;TreeKeyNode* current=mRoot.get();int depth=1;
    for(int node:nodes) {
        if(!current) throw std::logic_error("TreeKey private tree is not initialized");
        const auto& wots=current->getWOTSKey(node);auto pubkey=wots.getPublicKey().value();auto proof=current->getProof(node);
        if(depth==mLevels){if(!data)throw JavaNullPointerException("null signing data");signature.addSignatureProof(SignatureProof(pubkey,wots.sign(*data),std::move(proof)));}
        else {
            auto& child=current->getChild(node);
            if(!child.childSigExists()) child.setParentChildSig(SignatureProof(pubkey,wots.sign(child.getPublicKey()),std::move(proof)));
            signature.addSignatureProof(child.getParentChildSig().value());current=&child;
        }
        ++depth;
    }
    mUses=std::bit_cast<int32_t>(uint32_t(mUses)+1u);return signature;
}
bool TreeKey::verify(const MiniData& data,const Signature* signature) const {return verify(&data,signature);}
bool TreeKey::verify(const MiniData* data,const Signature* signature) const {
    if(!signature)throw JavaNullPointerException("Cannot invoke \"org.minima.objects.keys.Signature.getAllSignatureProofs()\" because \"<parameter2>\" is null");
    return verify(data,*signature);
}
bool TreeKey::verify(const MiniData* data,const Signature& signature) const {
    const auto proofs=signature.getAllSignatureProofs();if(proofs->size()>MAX_KEY_LEVELS){log("[!] INVALID KEY found with "+std::to_string(proofs->size())+" levels MAX:"+std::to_string(MAX_KEY_LEVELS));return false;}
    for(size_t depth=0;depth<proofs->size();++depth) {
        const auto& proof=(*proofs)[depth];if(depth==0) {auto root=proof.getRootPublicKey();if(!mPublicKey)throw JavaNullPointerException("");if(!root.isEqual(*mPublicKey))return false;}
        if(depth==proofs->size()-1) {if(!data)throw JavaNullPointerException("null verification data");return Winternitz::verify(proof.getPublicKey(),*data,proof.getSignature());}
        if(!Winternitz::verify(proof.getPublicKey(),(*proofs)[depth+1].getRootPublicKey(),proof.getSignature())) return false;
    }
    return false;
}
}
