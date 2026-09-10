#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <memory>
#include <openssl/evp.h>
namespace minima {
class CryptoKey {
    std::shared_ptr<EVP_PKEY> mKey;
    bool mPrivate;
public:
    CryptoKey(EVP_PKEY* key,bool priv):mKey(key,EVP_PKEY_free),mPrivate(priv) {if(!key) throw std::invalid_argument("Invalid RSA key");}
    EVP_PKEY* native() const {return mKey.get();}
    bool isPrivate() const {return mPrivate;}
    std::vector<uint8_t> getEncoded() const;
};
struct CryptoKeyPair {CryptoKey publicKey,privateKey;const CryptoKey& getPublic() const {return publicKey;}const CryptoKey& getPrivate() const {return privateKey;}};
struct SecretKey {std::vector<uint8_t> bytes;const std::vector<uint8_t>& getEncoded() const {return bytes;}};
class Cipher {
    bool mSymmetric;
    int mMode=0;
    std::optional<CryptoKey> mRSA;
    std::vector<uint8_t> mSecret,mIV;
    std::shared_ptr<EVP_CIPHER_CTX> mContext;
    mutable size_t mInputBytes=0,mOutputBytes=0;
    mutable std::vector<uint8_t> mPendingOutput;
public:
    static constexpr int ENCRYPT_MODE=1,DECRYPT_MODE=2;
    explicit Cipher(bool symmetric):mSymmetric(symmetric){}
    Cipher(const Cipher&)=delete;
    Cipher& operator=(const Cipher&)=delete;
    Cipher(Cipher&&)=default;
    Cipher& operator=(Cipher&&)=default;
    void init(int mode,CryptoKey key);
    void init(int mode,const SecretKey& key,std::span<const uint8_t> iv);
    std::vector<uint8_t> update(std::span<const uint8_t> input) const;
    std::vector<uint8_t> doFinal(std::span<const uint8_t> input) const;
};
class GenerateKey {
public:
    static constexpr const char* ASYMETRIC_ALGORITHM_GEN="RSA";
    static constexpr const char* ASYMETRIC_ALGORITHM="RSA/ECB/PKCS1Padding";
    static CryptoKeyPair generateKeyPair();
    static CryptoKey convertBytesToPublic(std::span<const uint8_t> bytes);
    static CryptoKey convertBytesToPrivate(std::span<const uint8_t> bytes);
    static std::vector<uint8_t> secretKey();
    static SecretKey convertSecret(std::span<const uint8_t> secret);
    static SecretKey secretKey(const std::string& password,std::span<const uint8_t> salt);
    static std::vector<uint8_t> IvParam();
    static Cipher getAsymetricCipher() {return Cipher(false);}
    static Cipher getSymetricCipher() {return Cipher(true);}
    static Cipher getCipherSYM(int mode,std::span<const uint8_t> iv,std::span<const uint8_t> secret);
};
}
