#include "SPHINCS.hpp"
#include "FORS/FORS.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
namespace minima::sphincs {
namespace {
template<class T> const T& required(const T* value) {if(!value)throw JavaNullPointerException("");return *value;}
}
void SPHINCS::initSeed(const MiniData* seed) {
    auto wots=required(seed).concat(MiniData::fromHex("574F5453")); // default UTF-8 bytes of WOTS
    mWotsSeed=std::make_shared<MiniData>(Crypto::getInstance().hashData(wots.bytes()));
    auto fors=seed->concat(MiniData::fromHex("464F5253"));
    mForsBaseSeed=std::make_shared<MiniData>(Crypto::getInstance().hashData(fors.bytes()));
    mPrivateKey=std::make_shared<MiniData>(mWotsSeed->concat(*mForsBaseSeed));
    TreeKey key(*mWotsSeed,WOTS_KEYSPERLEVEL,WOTS_DEPTH);
    mPublicKey=std::make_shared<MiniData>(key.getPublicKey().value());
    mWotsKeyNum=std::make_shared<BigInt>(std::to_string(key.getMaxUses()));
}
void SPHINCS::initPrivateKey(std::shared_ptr<MiniData> key) {
    mPrivateKey=std::move(key); // Java exposes this assignment even if copying fails.
    const auto& bytes=required(mPrivateKey.get()).bytes();
    std::vector<uint8_t> wots(32),fors(32);
    auto at=[&](size_t i) {if(i>=bytes.size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(i)+" out of bounds for length "+std::to_string(bytes.size()));return bytes[i];};
    for(size_t i=0;i<32;++i) {wots[i]=at(i);fors[i]=at(i+32);}
    mWotsSeed=std::make_shared<MiniData>(std::move(wots));
    mForsBaseSeed=std::make_shared<MiniData>(std::move(fors));
    TreeKey tree(*mWotsSeed,WOTS_KEYSPERLEVEL,WOTS_DEPTH);
    mPublicKey=std::make_shared<MiniData>(tree.getPublicKey().value());
    mWotsKeyNum=std::make_shared<BigInt>(std::to_string(tree.getMaxUses()));
}
SPHINCSSignature SPHINCS::signMessage(const MiniData* message,std::shared_ptr<MiniData> privateKey) {
    SPHINCS signer;signer.initPrivateKey(std::move(privateKey));return signer.signMessage(message);
}
SPHINCSSignature SPHINCS::signMessage(const MiniData* message) const {
    MiniData hash(Crypto::getInstance().hashData(required(message).bytes()));
    auto index=hash.getDataValue()%required(mWotsKeyNum.get());
    int use=std::stoi(index.toDecimalString()); // The positive remainder is below 192^4 < INT_MAX.
    TreeKey tree(required(mWotsSeed.get()),WOTS_KEYSPERLEVEL,WOTS_DEPTH);tree.setUses(use);
    auto encodedIndex=MiniData::fromHex(std::to_string(use)); // Decimal text is parsed as HEX by Java.
    auto unique=required(mForsBaseSeed.get()).concat(encodedIndex);
    MiniData seed(Crypto::getInstance().hashData(unique.bytes()));
    fors::FORS forest(seed);auto root=forest.getForsRoot();
    auto wots=std::make_shared<Signature>(tree.sign(root->getData()));
    auto signature=std::make_shared<fors::FORSSignature>(forest.signMessage(message));
    return SPHINCSSignature(std::move(wots),std::move(root),std::move(signature));
}
bool SPHINCS::verifySignature(const MiniData* message,const SPHINCSSignature* signature,const MiniData* publicKey) {
    auto root=required(signature).getFORSRoot();auto fors=signature->getFORSSignature();auto wots=signature->getWOTSSignature();
    TreeKey tree;tree.setPublicKey(publicKey);
    const auto& data=required(root.get()).getData();
    if(!tree.verify(data,required(wots.get()))) {log("SPHINCS VERIFY wots  : FALSE");return false;}
    if(!fors::FORS::verifySignature(message,fors.get(),root.get())) {log("SPHINCS VERIFY fors  : FALSE");return false;}
    return true;
}
void SPHINCS::log(const std::optional<std::string>& message) {std::cout<<message.value_or("null")<<std::endl;}
void SPHINCS::main() {
    auto seed=MiniData::fromHex("0x0011223344"),message=MiniData::fromHex("0x998877661");log("Start SPHINCS..");
    SPHINCS signer;signer.initSeed(seed);log("SPHINCS public key : "+signer.getPublicKey()->to0xString());
    log("SPHINCS total WOTS keys : "+signer.getTotalWotsKeys()->toDecimalString());
    auto sig=signer.signMessage(message);MiniData stream(sig.serialise());auto read=SPHINCSSignature::convertMiniDataVersion(stream);
    log("Signature Size : "+std::to_string(stream.getLength()));
    log(std::string("Verify : ")+(verifySignature(message,read,*signer.getPublicKey())?"true":"false"));
}
}
