#include "EncryptDecrypt.hpp"
#include "support/ByteIO.hpp"
#include <iostream>
namespace minima {
std::vector<uint8_t> EncryptDecrypt::encryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data) {auto cipher=GenerateKey::getAsymetricCipher();cipher.init(Cipher::ENCRYPT_MODE,GenerateKey::convertBytesToPublic(key));return cipher.doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::decryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data) {auto cipher=GenerateKey::getAsymetricCipher();cipher.init(Cipher::DECRYPT_MODE,GenerateKey::convertBytesToPrivate(key));return cipher.doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::encryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data) {return GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE,iv,key).doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::decryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data) {return GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv,key).doFinal(data);}
void EncryptDecrypt::main(const std::vector<std::string>*) {
    auto data=MiniData::getRandomData(1024);const std::string password="hello";
    auto salt=MiniData::getRandomData(8);MiniData iv(GenerateKey::IvParam());
    auto secret=GenerateKey::secretKey(password,salt.bytes());
    auto cipher=GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE,iv.bytes(),secret.bytes);
    auto bytes=wire::join(salt,iv);wire::append(bytes,cipher.update(data.bytes()));
    wire::append(bytes,cipher.doFinal({}));MiniData encrypted(bytes);
    std::cout<<"Orig : "<<data.to0xString()<<'\n'<<"Salt : "<<salt.to0xString()<<'\n'
             <<"IVPa : "<<iv.to0xString()<<'\n'<<"Len  : "<<data.getLength()<<'\n'
             <<"Enc  : "<<encrypted.to0xString()<<'\n'<<"Len  : "<<encrypted.getLength()<<'\n';
    size_t offset=0;auto inputSalt=MiniData::deserialise(bytes,offset);auto inputIV=MiniData::deserialise(bytes,offset);
    auto inputSecret=GenerateKey::secretKey(password,inputSalt.bytes());
    [[maybe_unused]] auto inputCipher=GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,inputIV.bytes(),inputSecret.bytes);
    // The source constructs CipherInputStream but its read and output are commented out.
}

}
