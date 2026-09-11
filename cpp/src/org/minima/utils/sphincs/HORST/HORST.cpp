#include "HORST.hpp"
#include "support/JavaExceptions.hpp"
#include <bit>
#include <iostream>
namespace minima::sphincs::horst {
namespace {
template<class T> const T& required(const T* value) {if(!value)throw JavaNullPointerException("");return *value;}
template<class T> const T& listAt(const std::vector<std::shared_ptr<T>>& list,int position) {
    if(size_t(position)>=list.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(position)+" out of bounds for length "+std::to_string(list.size()));
    return required(list[size_t(position)].get());
}
}
HORST::HORST(const MiniData* seed) {
    mPrivateKey=std::make_shared<PrivateKey>(seed);mPublicKey=std::make_shared<PublicKey>(*mPrivateKey);
}
HORSTSignature HORST::signMessage(const MiniData* message) const {
    MiniData hashed(Crypto::getInstance().hashData(required(message).bytes()));HORSTSignature signature;
    for(int i=0;i<NUMBER_OF_CHUNK_BYTES;++i) {
        int ref=getKeyRef(i,hashed);signature.getSignatureValues()->push_back(mPrivateKey->getKey(ref));
        signature.getPublicKeyTreeProofs()->push_back(std::make_shared<MMRProof>(mPublicKey->getKeyTreeProof(ref)));
    }
    return signature;
}
bool HORST::verifySignature(const MiniData* message,const HORSTSignature* signature,const MMRData* root) {
    MiniData hashed(Crypto::getInstance().hashData(required(message).bytes()));
    for(int i=0;i<NUMBER_OF_CHUNK_BYTES;++i) {
        int ref=getKeyRef(i,hashed);
        const auto& preimage=listAt(*required(signature).getSignatureValues(),i);
        MiniData publicKey(Crypto::getInstance().hashData(preimage.bytes()));
        auto leaf=MMRData::CreateMMRDataLeafNode(publicKey,MiniNumber(ref));
        auto check=listAt(*signature->getPublicKeyTreeProofs(),i).calculateProof(leaf);
        if(!check.isEqual(required(root)))return false;
    }
    return true;
}
int HORST::getKeyRef(int position,const MiniData* original) {
    const auto& bytes=required(original).bytes();int32_t offset=std::bit_cast<int32_t>(uint32_t(position)*uint32_t(2));
    auto at=[&](int32_t i) {if(i<0||uint64_t(i)>=bytes.size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(i)+" out of bounds for length "+std::to_string(bytes.size()));return bytes[size_t(i)];};
    auto first=at(offset);auto second=at(std::bit_cast<int32_t>(uint32_t(offset)+uint32_t(1)));
    return int(first)*256+int(second);
}
void HORST::log(const std::optional<std::string>& message) {std::cout<<message.value_or("null")<<std::endl;}
void HORST::main() {
    log("Start HORST Test");auto message=MiniData::fromHex("0xFFEEDD");log("Message : "+message.to0xString());
    auto seed=MiniData::fromHex("0x00112233");log("Generate HORST signing key..");HORST horst(seed);
    auto root=horst.getPublicKey()->getPublicKeyTreeRoot();log("HORST private key size.. "+std::to_string(horst.getPrivateKey()->getSize()));
    log("HORST public key root.. "+root->toString());log("Sign message..");auto signature=horst.signMessage(message);
    MiniData stream(signature.serialise());auto read=HORSTSignature::convertMiniDataVersion(stream);log("Signature Size : "+std::to_string(stream.getLength()));
    log("Signature : ");std::cout<<std::endl;log(std::string("Verify : ")+(verifySignature(message,read,*root)?"true":"false"));
}
}
