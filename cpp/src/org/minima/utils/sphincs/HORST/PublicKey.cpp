#include "PublicKey.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::sphincs::horst {
PublicKey::PublicKey(const PrivateKey* privateKey) {
    if(!privateKey)throw JavaNullPointerException("");
    int count=privateKey->getSize();mPublicKeyTree=std::make_shared<MMR>();mPublicKeys.resize(size_t(count));
    for(int i=0;i<count;++i) {
        auto secret=privateKey->getKey(i);if(!secret)throw JavaNullPointerException("");
        mPublicKeys[i]=std::make_shared<MiniData>(Crypto::getInstance().hashData(secret->bytes()));
        mPublicKeyTree->addEntry(MMRData::CreateMMRDataLeafNode(*mPublicKeys[i],MiniNumber(i)));
    }
    mPublicKeyTree->finalizeSet();mPublicKeyTreeRoot=mPublicKeyTree->getRoot();
}
std::shared_ptr<MiniData> PublicKey::getKey(int position) const {
    if(position<0||position>=getSize())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(position)+" out of bounds for length "+std::to_string(getSize()));
    return mPublicKeys[size_t(position)];
}
MMRProof PublicKey::getKeyTreeProof(int position) const {
    // A complete 2^16 tree has one peak. In Java, an absent leaf with no
    // sibling passes null through two empty proof loops and returns no chunks.
    // The general native MMR API rejects missing leaves; preserve this caller's
    // source behavior without changing other MMR users.
    if(position<0||position>=getSize())return mPublicKeyTree->getProofToPeak(MMREntryNumber(position));
    return mPublicKeyTree->getProof(MMREntryNumber(position));
}
}
