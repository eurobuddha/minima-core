#include "SignVerify.hpp"
#include <openssl/rsa.h>
namespace minima {
std::vector<uint8_t> SignVerify::sign(std::span<const uint8_t> key,std::span<const uint8_t> message) {
    auto privateKey=GenerateKey::convertBytesToPrivate(key);std::unique_ptr<EVP_MD_CTX,decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_new(),EVP_MD_CTX_free);if(!ctx) throw std::bad_alloc();
    if(EVP_DigestSignInit(ctx.get(),nullptr,EVP_sha256(),nullptr,privateKey.native())<=0 || EVP_DigestSignUpdate(ctx.get(),message.data(),message.size())<=0) throw std::runtime_error("RSA signing init");
    size_t length=0;if(EVP_DigestSignFinal(ctx.get(),nullptr,&length)<=0) throw std::runtime_error("RSA signing size");std::vector<uint8_t> signature(length);if(EVP_DigestSignFinal(ctx.get(),signature.data(),&length)<=0) throw std::runtime_error("RSA signing");signature.resize(length);return signature;
}
bool SignVerify::verify(std::span<const uint8_t> key,std::span<const uint8_t> message,std::span<const uint8_t> signature) {
    auto publicKey=GenerateKey::convertBytesToPublic(key);if(signature.size()!=size_t(EVP_PKEY_get_size(publicKey.native()))) throw std::invalid_argument("Bad signature length");
    std::unique_ptr<EVP_MD_CTX,decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_new(),EVP_MD_CTX_free);if(!ctx) throw std::bad_alloc();
    if(EVP_DigestVerifyInit(ctx.get(),nullptr,EVP_sha256(),nullptr,publicKey.native())<=0 || EVP_DigestVerifyUpdate(ctx.get(),message.data(),message.size())<=0) throw std::runtime_error("RSA verify init");
    int result=EVP_DigestVerifyFinal(ctx.get(),signature.data(),signature.size());if(result<0) throw std::runtime_error("RSA verification error");return result==1;
}
}
