#include "GenerateKey.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "support/JavaExceptions.hpp"
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <limits>
namespace minima {
namespace {
void checked(int result,const char* action) {if(result<=0) throw std::runtime_error(action);}
int sizeInt(size_t size) {if(size>size_t(std::numeric_limits<int>::max())) throw std::length_error("Crypto input too large");return int(size);}
using KeyContext=std::unique_ptr<EVP_PKEY_CTX,decltype(&EVP_PKEY_CTX_free)>;
}
std::vector<uint8_t> CryptoKey::getEncoded() const {
    if(!mPrivate) {int length=i2d_PUBKEY(native(),nullptr);checked(length,"RSA public encoding");std::vector<uint8_t> bytes(length);auto p=bytes.data();checked(i2d_PUBKEY(native(),&p),"RSA public encoding");return bytes;}
    std::unique_ptr<PKCS8_PRIV_KEY_INFO,decltype(&PKCS8_PRIV_KEY_INFO_free)> info(EVP_PKEY2PKCS8(native()),PKCS8_PRIV_KEY_INFO_free);if(!info) throw std::runtime_error("RSA private encoding");
    int length=i2d_PKCS8_PRIV_KEY_INFO(info.get(),nullptr);checked(length,"RSA private encoding");std::vector<uint8_t> bytes(length);auto p=bytes.data();checked(i2d_PKCS8_PRIV_KEY_INFO(info.get(),&p),"RSA private encoding");return bytes;
}
CryptoKeyPair GenerateKey::generateKeyPair() {
    KeyContext ctx(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA,nullptr),EVP_PKEY_CTX_free);if(!ctx) throw std::bad_alloc();checked(EVP_PKEY_keygen_init(ctx.get()),"RSA keygen init");checked(EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(),1024),"RSA key size");
    EVP_PKEY* key=nullptr;checked(EVP_PKEY_keygen(ctx.get(),&key),"RSA keygen");CryptoKey priv(key,true);checked(EVP_PKEY_up_ref(key),"RSA reference");auto encoded=CryptoKey(key,false);return {convertBytesToPublic(encoded.getEncoded()),std::move(priv)};
}
CryptoKey GenerateKey::convertBytesToPublic(std::span<const uint8_t> bytes) {auto p=bytes.data();auto key=d2i_PUBKEY(nullptr,&p,long(bytes.size()));CryptoKey result(key,false);if(p!=bytes.data()+bytes.size()) throw std::invalid_argument("Trailing public key data");if(!EVP_PKEY_is_a(key,"RSA")) throw std::invalid_argument("Not an RSA public key");return result;}
CryptoKey GenerateKey::convertBytesToPrivate(std::span<const uint8_t> bytes) {
    auto p=bytes.data();std::unique_ptr<PKCS8_PRIV_KEY_INFO,decltype(&PKCS8_PRIV_KEY_INFO_free)> info(d2i_PKCS8_PRIV_KEY_INFO(nullptr,&p,long(bytes.size())),PKCS8_PRIV_KEY_INFO_free);if(!info || p!=bytes.data()+bytes.size()) throw std::invalid_argument("Invalid PKCS8 private key");
    auto key=EVP_PKCS82PKEY(info.get());CryptoKey result(key,true);if(!EVP_PKEY_is_a(key,"RSA")) throw std::invalid_argument("Not an RSA private key");return result;
}
std::vector<uint8_t> GenerateKey::secretKey() {return MiniData::getRandomData(16).bytes();}
SecretKey GenerateKey::convertSecret(std::span<const uint8_t> secret) {if(secret.empty()) throw std::invalid_argument("Empty key");return SecretKey{std::vector<uint8_t>(secret.begin(),secret.end())};}
SecretKey GenerateKey::secretKey(const std::string& password,std::span<const uint8_t> salt) {
    if(salt.empty()) throw JavaIllegalArgumentException("the salt parameter must not be empty");auto utf8=MiniString(password).toString();std::vector<uint8_t> key(16);
    checked(PKCS5_PBKDF2_HMAC(utf8.data(),sizeInt(utf8.size()),salt.data(),sizeInt(salt.size()),65536,EVP_sha256(),16,key.data()),"PBKDF2");return SecretKey{std::move(key)};
}
std::vector<uint8_t> GenerateKey::IvParam() {return MiniData::getRandomData(16).bytes();}
Cipher GenerateKey::getCipherSYM(int mode,std::span<const uint8_t> iv,std::span<const uint8_t> secret) {auto cipher=getSymetricCipher();cipher.init(mode,convertSecret(secret),iv);return cipher;}
void Cipher::init(int mode,CryptoKey key) {if(mSymmetric || (mode!=ENCRYPT_MODE && mode!=DECRYPT_MODE)) throw std::invalid_argument("Invalid RSA mode");mRSA=std::move(key);mMode=mode;}
void Cipher::init(int mode,const SecretKey& key,std::span<const uint8_t> iv) {
    if(!mSymmetric || (mode!=ENCRYPT_MODE && mode!=DECRYPT_MODE)) throw std::invalid_argument("Invalid AES mode");
    if(key.bytes.size()!=16 && key.bytes.size()!=24 && key.bytes.size()!=32)throw std::invalid_argument("Invalid AES key or IV");
    if(iv.size()!=16)throw JavaTypedException("java.security.InvalidAlgorithmParameterException","Wrong IV length: must be 16 bytes long");
    mMode=mode;mSecret=key.bytes;mIV.assign(iv.begin(),iv.end());
    mContext={EVP_CIPHER_CTX_new(),EVP_CIPHER_CTX_free};if(!mContext) throw std::bad_alloc();
    auto algorithm=mSecret.size()==16?EVP_aes_128_cbc():mSecret.size()==24?EVP_aes_192_cbc():EVP_aes_256_cbc();
    checked(EVP_CipherInit_ex(mContext.get(),algorithm,nullptr,mSecret.data(),mIV.data(),mMode==ENCRYPT_MODE),"AES init");
    mInputBytes=0;mOutputBytes=0;mPendingOutput.clear();
}
std::vector<uint8_t> Cipher::update(std::span<const uint8_t> input) const {
    if(!mMode || !mSymmetric || !mContext) throw std::logic_error("AES cipher not initialized");
    int length=0;std::vector<uint8_t> out(input.size()+16);
    checked(EVP_CipherUpdate(mContext.get(),out.data(),&length,input.data(),sizeInt(input.size())),"AES update");out.resize(length);
    mInputBytes+=input.size();mPendingOutput.insert(mPendingOutput.end(),out.begin(),out.end());
    // SunJCE CipherCore holds one full padding block AND any partial block.
    // OpenSSL may expose that full block early when a partial tail is present.
    size_t available=(mMode==DECRYPT_MODE?(mInputBytes>16?mInputBytes-16:0):mInputBytes)/16*16;
    size_t emit=available-mOutputBytes;if(emit>mPendingOutput.size()) throw std::logic_error("AES buffering mismatch");
    out.assign(mPendingOutput.begin(),mPendingOutput.begin()+emit);mPendingOutput.erase(mPendingOutput.begin(),mPendingOutput.begin()+emit);mOutputBytes+=emit;return out;
}
std::vector<uint8_t> Cipher::doFinal(std::span<const uint8_t> input) const {
    if(!mMode) throw std::logic_error("Cipher not initialized");
    if(mSymmetric) {
        // SunJCE accepts an empty padded decryption when no update data exists.
        // OpenSSL's padding finalizer rejects it; no cipher state was consumed.
        if(mMode==DECRYPT_MODE && mInputBytes==0 && input.empty()) return {};
        auto out=update(input);out.insert(out.end(),mPendingOutput.begin(),mPendingOutput.end());auto length=out.size();out.resize(length+16);int final=0;
        if(EVP_CipherFinal_ex(mContext.get(),out.data()+length,&final)!=1){
            if(mMode==DECRYPT_MODE){
                if(mInputBytes%16)throw JavaTypedException("javax.crypto.IllegalBlockSizeException","Input length must be multiple of 16 when decrypting with padded cipher");
                throw JavaTypedException("javax.crypto.BadPaddingException","Given final block not properly padded. Such issues can arise if a bad key is used during decryption.");
            }
            throw std::runtime_error("AES padding");
        }
        out.resize(length+final);
        checked(EVP_CipherInit_ex(mContext.get(),nullptr,nullptr,mSecret.data(),mIV.data(),mMode==ENCRYPT_MODE),"AES reset");mInputBytes=0;mOutputBytes=0;mPendingOutput.clear();return out;
    }
    auto key=mRSA.value().native();KeyContext ctx(EVP_PKEY_CTX_new(key,nullptr),EVP_PKEY_CTX_free);if(!ctx) throw std::bad_alloc();
    bool encrypt=mMode==ENCRYPT_MODE;
    if(encrypt==mRSA->isPrivate()) throw std::invalid_argument("Unsupported RSA key direction");
    checked(encrypt?EVP_PKEY_encrypt_init(ctx.get()):EVP_PKEY_decrypt_init(ctx.get()),"RSA init");checked(EVP_PKEY_CTX_set_rsa_padding(ctx.get(),RSA_PKCS1_PADDING),"RSA padding mode");
    if(!encrypt) {unsigned int reject=0;OSSL_PARAM params[]={OSSL_PARAM_construct_uint(OSSL_ASYM_CIPHER_PARAM_IMPLICIT_REJECTION,&reject),OSSL_PARAM_construct_end()};checked(EVP_PKEY_CTX_set_params(ctx.get(),params),"RSA explicit padding errors");}
    size_t length=0;checked(encrypt?EVP_PKEY_encrypt(ctx.get(),nullptr,&length,input.data(),input.size()):EVP_PKEY_decrypt(ctx.get(),nullptr,&length,input.data(),input.size()),"RSA output size");std::vector<uint8_t> out(length);
    checked(encrypt?EVP_PKEY_encrypt(ctx.get(),out.data(),&length,input.data(),input.size()):EVP_PKEY_decrypt(ctx.get(),out.data(),&length,input.data(),input.size()),"RSA operation");out.resize(length);return out;
}
}
