#include "AesUtil.hpp"
#include "support/Base64.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <limits>

namespace minima {
namespace {
const std::u16string& required(const AesUtil::Text& value) {
    if (!value) throw JavaNullPointerException("");
    return *value;
}
int sizeInt(size_t value) {
    if (value > size_t(std::numeric_limits<int>::max())) throw std::length_error("Crypto input too large");
    return int(value);
}
}
AesUtil::AesUtil(int bits, int iterations)
    : keySize(bits), iterationCount(iterations), cipher(GenerateKey::getSymetricCipher()) {}

std::optional<SecretKey> AesUtil::generateKey(const Text& salt, const Text& passphrase) {
    // Java evaluates passphrase.toCharArray() before hex(salt).
    auto password = unicode::encodeUTF8(required(passphrase));
    auto saltBytes = hex(salt);
    if (saltBytes.empty()) throw JavaIllegalArgumentException("the salt parameter must not be empty");
    if (iterationCount <= 0) throw JavaIllegalArgumentException("invalid iterationCount value");
    if (keySize <= 0) throw JavaIllegalArgumentException("invalid keyLength value");
    // SunJCE PBKDF2 uses keyLengthInBit / 8, even for non-byte-aligned sizes.
    std::vector<uint8_t> key(size_t(keySize / 8));
    if (key.empty()) throw JavaIllegalArgumentException("Empty key");
    if (PKCS5_PBKDF2_HMAC(password.data(), sizeInt(password.size()), saltBytes.data(),
            sizeInt(saltBytes.size()), iterationCount, EVP_sha1(), sizeInt(key.size()), key.data()) != 1)
        throw std::runtime_error("Error deriving PBKDF2 keys");
    return SecretKey{std::move(key)};
}

AesUtil::Bytes AesUtil::doFinal(int mode, const std::optional<SecretKey>& key, const Text& iv, const Bytes& bytes) {
    auto ivBytes = hex(iv);
    // Cipher.chooseProvider consumes its candidates until the first successful
    // init. A failed first init exhausts the supported SunJCE provider; later
    // init calls then fail too. After selection, argument failures can recover.
    if (providerExhausted) return std::nullopt;
    if (!key || (key->bytes.size() != 16 && key->bytes.size() != 24 && key->bytes.size() != 32) || ivBytes.size() != 16) {
        if (!cipherSelected) providerExhausted = true;
        return std::nullopt;
    }
    try {
        cipher.init(mode, *key, ivBytes);
        cipherSelected = true;
        if (!bytes) throw JavaIllegalArgumentException("Null input buffer");
        return cipher.doFinal(*bytes);
    } catch (const JavaTypedException& e) {
        if (e.className() == "java.security.InvalidAlgorithmParameterException" ||
            e.className() == "javax.crypto.IllegalBlockSizeException" ||
            e.className() == "javax.crypto.BadPaddingException") return std::nullopt;
        throw;
    }
}

AesUtil::Text AesUtil::encrypt(const Text& salt, const Text& iv, const Text& passphrase, const Text& plaintext) {
    try {
        auto key = generateKey(salt, passphrase);
        // The native runtime, like the supported JDK default, uses UTF-8.
        auto text = unicode::encodeUTF8(required(plaintext));
        return base64Encode(doFinal(Cipher::ENCRYPT_MODE, key, iv, std::vector<uint8_t>(text.begin(), text.end())));
    } catch (const JavaError&) { throw; }
      catch (const std::bad_alloc&) { throw; }
      catch (const std::exception&) { return std::nullopt; }
}

AesUtil::Text AesUtil::decrypt(const Text& salt, const Text& iv, const Text& passphrase, const Text& ciphertext) {
    try {
        auto key = generateKey(salt, passphrase);
        auto bytes = base64Decode(ciphertext);
        auto plain = doFinal(Cipher::DECRYPT_MODE, key, iv, bytes);
        if (!plain) throw JavaNullPointerException("");
        return unicode::decodeUTF8(MiniString(*plain).toString());
    } catch (const JavaError&) { throw; }
      catch (const std::bad_alloc&) { throw; }
      catch (const std::exception&) { return std::nullopt; }
}

std::vector<uint8_t> AesUtil::base64Decode(const Text& text) {
    return decodeBase64(unicode::encodeUTF8(required(text)));
}
std::u16string AesUtil::base64Encode(const Bytes& bytes) {
    if (!bytes) throw JavaNullPointerException("");
    return unicode::decodeUTF8(encodeBase64(*bytes));
}
std::vector<uint8_t> AesUtil::hex(const Text& text) {
    return MiniData::fromHex(unicode::encodeUTF8(required(text))).bytes();
}
void AesUtil::main(std::ostream& out) {
    AesUtil aes(128, 1000);
    Text salt = u"4452150bad3b58e9d2c043ad24db2b1d", iv = u"0d93cefd0147ecb48a379b5be0da7e8a";
    Text password = u"apasswordblabla", text = u"helloyou!";
    auto encrypted = aes.encrypt(salt, iv, password, text);
    out << (encrypted ? unicode::encodeUTF8(*encrypted) : "null") << '\n';
    auto plain = aes.decrypt(salt, iv, password, encrypted);
    out << (plain ? unicode::encodeUTF8(*plain) : "null") << '\n';
}
}
