#include "PasswordCrypto.hpp"
#include "support/Gzip.hpp"
namespace minima {
MiniData PasswordCrypto::encryptPassword(const std::string& password,const MiniData& data) {
    auto salt=MiniData::getRandomData(8);MiniData iv(GenerateKey::IvParam());auto secret=GenerateKey::secretKey(password,salt.bytes());auto out=wire::join(salt,iv);auto compressed=compressGzip(data.serialise());wire::append(out,GenerateKey::getCipherSYM(Cipher::ENCRYPT_MODE,iv.bytes(),secret.bytes).doFinal(compressed));return MiniData(std::move(out));
}
MiniData PasswordCrypto::decryptPassword(const std::string& password,const MiniData& encrypted) {
    size_t offset=0;auto salt=MiniData::deserialise(encrypted.bytes(),offset);auto iv=MiniData::deserialise(encrypted.bytes(),offset);auto secret=GenerateKey::secretKey(password,salt.bytes());
    auto cipher=GenerateKey::getCipherSYM(Cipher::DECRYPT_MODE,iv.bytes(),secret.bytes);
    auto ciphertext=std::span(encrypted.bytes()).subspan(offset);std::vector<uint8_t> decrypted;
    // CipherInputStream exposes update output before final padding validation.
    // PasswordCrypto reads one MiniData, then closes without consuming the tail.
    for(size_t position=0;position<ciphertext.size();) {
        auto length=std::min(size_t(8192),ciphertext.size()-position);
        wire::append(decrypted,cipher.update(ciphertext.subspan(position,length)));position+=length;
        try {auto uncompressed=decompressGzip(decrypted,true);size_t read=0;return MiniData::deserialise(uncompressed,read);}catch(const std::runtime_error&) {}catch(const std::out_of_range&) {}
    }
    wire::append(decrypted,cipher.doFinal({}));auto uncompressed=decompressGzip(decrypted,true);offset=0;return MiniData::deserialise(uncompressed,offset);
}
}
