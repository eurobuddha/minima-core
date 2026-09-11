#include "CryptoPackage.hpp"
#include <iostream>
namespace minima {
void CryptoPackage::encrypt(std::span<const uint8_t> data,std::span<const uint8_t> publicKey) {mIvParam=MiniData(GenerateKey::IvParam());auto secret=GenerateKey::secretKey();mData=MiniData(EncryptDecrypt::encryptSYM(mIvParam->bytes(),secret,data));mSecret=MiniData(EncryptDecrypt::encryptASM(publicKey,secret));}
std::vector<uint8_t> CryptoPackage::decrypt(std::span<const uint8_t> privateKey) const {auto secret=EncryptDecrypt::decryptASM(privateKey,mSecret.value().bytes());return EncryptDecrypt::decryptSYM(mIvParam.value().bytes(),secret,mData.value().bytes());}
void CryptoPackage::ConvertMiniDataVersion(const MiniData& data) {size_t offset=0;readDataStream(data.bytes(),offset);}
std::vector<uint8_t> CryptoPackage::serialise() const {return wire::join(mIvParam.value(),mSecret.value(),mData.value());}
void CryptoPackage::readDataStream(std::span<const uint8_t> data,size_t& offset) {mIvParam=MiniData::deserialise(data,offset);mSecret=MiniData::deserialise(data,offset);mData=MiniData::deserialise(data,offset);}
CryptoPackage CryptoPackage::deserialise(std::span<const uint8_t> data,size_t& offset) {CryptoPackage object;object.readDataStream(data,offset);return object;}
void CryptoPackage::main(const std::vector<std::string>*) {
    // See SignVerify::main for the Java provider-registration adaptation.
    auto key=GenerateKey::generateKeyPair();auto publicKey=key.getPublic().getEncoded();
    MiniData publicData(publicKey);auto privateKey=key.getPrivate().getEncoded();MiniData privateData(privateKey);
    auto data=MiniData::getRandomData(256);CryptoPackage package;package.encrypt(data.bytes(),publicKey);
    std::cout<<"Public Key  : "<<publicData.getLength()<<'\n'
             <<"Private Key : "<<privateData.getLength()<<'\n'
             <<"Secret Key  : "<<package.mSecret.value().getLength()<<'\n'
             <<"Data        : "<<data.getLength()<<'\n'
             <<"Enc Data    : "<<package.getCompleteEncryptedData().getLength()<<'\n';
    MiniData decrypted(package.decrypt(privateKey));std::cout<<"Worked     : "<<(decrypted.isEqual(data)?"true":"false")<<'\n';
}

}
