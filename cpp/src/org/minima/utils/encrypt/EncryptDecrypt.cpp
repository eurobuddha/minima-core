#include "EncryptDecrypt.hpp"
namespace minima {
std::vector<uint8_t> EncryptDecrypt::encryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data) {auto cipher=GenerateKey::getAsymetricCipher();cipher.init(Cipher::ENCRYPT_MODE,GenerateKey::convertBytesToPublic(key));return cipher.doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::decryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data) {auto cipher=GenerateKey::getAsymetricCipher();cipher.init(Cipher::DECRYPT_MODE,GenerateKey::convertBytesToPrivate(key));return cipher.doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::encryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data) {return GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE,iv,key).doFinal(data);}
std::vector<uint8_t> EncryptDecrypt::decryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data) {return GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv,key).doFinal(data);}
}
