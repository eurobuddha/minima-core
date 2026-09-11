#include "FORS.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
namespace minima::sphincs::fors {
namespace {
template<class T> const T& required(const T* value) {if(!value)throw JavaNullPointerException("");return *value;}
template<class T> const T& at(const std::vector<std::shared_ptr<T>>& list,size_t index) {
    if(index>=list.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(index)+" out of bounds for length "+std::to_string(list.size()));
    return required(list[index].get());
}
}
FORS::FORS(const MiniData* seed):mForestHORS(std::make_shared<MMR>()) {
    for(int i=0;i<horst::HORST::NUMBER_OF_CHUNK_BYTES;++i) {
        std::optional<MiniData> childSeed;
        if(seed)childSeed=Crypto::getInstance().hashAllObjects(*seed,MiniNumber(i));
        else {JavaNullPointerException error("");MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
        mHORSTLeafNodes[i]=std::make_shared<horst::HORST>(childSeed?&*childSeed:nullptr);
        auto root=mHORSTLeafNodes[i]->getHORSTRoot();
        mForestHORS->addEntry(MMRData::CreateMMRDataLeafNode(root->getData(),MiniNumber(i)));
    }
    mForestHORS->finalizeSet();
}
FORSSignature FORS::signMessage(const MiniData* message) const {
    MiniData hashed(Crypto::getInstance().hashData(required(message).bytes()));FORSSignature signature;
    for(int i=0;i<horst::HORST::NUMBER_OF_CHUNK_BYTES;++i) {
        const auto& tree=mHORSTLeafNodes[i];int ref=horst::HORST::getKeyRef(i,hashed);
        signature.getHORSTSignature()->getSignatureValues()->push_back(tree->getPrivateKey()->getKey(ref));
        signature.getHORSTSignature()->getPublicKeyTreeProofs()->push_back(std::make_shared<MMRProof>(tree->getPublicKey()->getKeyTreeProof(ref)));
        signature.getHORSTRoots()->push_back(tree->getHORSTRoot());
        signature.getHORSTTreeProofs()->push_back(std::make_shared<MMRProof>(mForestHORS->getProof(MMREntryNumber(i))));
    }
    return signature;
}
bool FORS::verifySignature(const MiniData* message,const FORSSignature* signature,const MMRData* root) {
    MiniData hashed(Crypto::getInstance().hashData(required(message).bytes()));
    for(int i=0;i<horst::HORST::NUMBER_OF_CHUNK_BYTES;++i) {
        const auto& horstRoot=at(*required(signature).getHORSTRoots(),size_t(i));
        auto leaf=MMRData::CreateMMRDataLeafNode(horstRoot.getData(),MiniNumber(i));
        auto outer=at(*signature->getHORSTTreeProofs(),size_t(i)).calculateProof(leaf);
        if(!outer.isEqual(required(root)))return false;
        int ref=horst::HORST::getKeyRef(i,hashed);
        const auto& preimage=at(*signature->getHORSTSignature()->getSignatureValues(),size_t(i));
        MiniData publicKey(Crypto::getInstance().hashData(preimage.bytes()));
        auto innerLeaf=MMRData::CreateMMRDataLeafNode(publicKey,MiniNumber(ref));
        auto inner=at(*signature->getHORSTSignature()->getPublicKeyTreeProofs(),size_t(i)).calculateProof(innerLeaf);
        if(!inner.isEqual(horstRoot))return false;
    }
    return true;
}
void FORS::log(const std::optional<std::string>& message) {std::cout<<message.value_or("null")<<std::endl;}
void FORS::main() {
    log("Start FORS Test");auto message=MiniData::fromHex("0xFFEEDD");log("Message : "+message.to0xString());
    auto seed=MiniData::fromHex("0x00112233");log("Generate FORS signing key..");FORS fors(seed);auto root=fors.getForsRoot();
    log("FORS public key root.. "+root->toString());log("Sign message..");auto signature=fors.signMessage(message);
    MiniData stream(signature.serialise());auto read=FORSSignature::convertMiniDataVersion(stream);log("Signature Size : "+std::to_string(stream.getLength()));
    log("Signature : ");std::cout<<std::endl;log(std::string("Verify : ")+(verifySignature(message,read,*root)?"true":"false"));
}
}
