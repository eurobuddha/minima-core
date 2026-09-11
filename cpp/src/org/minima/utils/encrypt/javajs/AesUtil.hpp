#pragma once
#include "org/minima/utils/encrypt/GenerateKey.hpp"
#include <ostream>

namespace minima {
// Nullable UTF-16 preserves Java String inputs, including unpaired surrogates.
class AesUtil {
public:
    using Text = std::optional<std::u16string>;
    using Bytes = std::optional<std::vector<uint8_t>>;
    AesUtil(int keySize, int iterationCount);
    Text encrypt(const Text& salt, const Text& iv, const Text& passphrase, const Text& plaintext);
    Text decrypt(const Text& salt, const Text& iv, const Text& passphrase, const Text& ciphertext);
    static std::vector<uint8_t> base64Decode(const Text& str);
    static std::u16string base64Encode(const Bytes& data);
    static std::vector<uint8_t> hex(const Text& str);
    static void main(std::ostream& out);
private:
    const int keySize;
    const int iterationCount;
    Cipher cipher;
    bool cipherSelected = false;
    bool providerExhausted = false;
    std::optional<SecretKey> generateKey(const Text& salt, const Text& passphrase);
    Bytes doFinal(int encryptMode, const std::optional<SecretKey>& key, const Text& iv, const Bytes& bytes);
    static std::exception_ptr fail(const std::exception&) { return nullptr; }
    friend class AesUtilFixture;
};
}
