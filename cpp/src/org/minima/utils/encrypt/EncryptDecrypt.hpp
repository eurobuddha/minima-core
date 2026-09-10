#pragma once
#include "GenerateKey.hpp"
namespace minima {
class EncryptDecrypt {
public:
    static std::vector<uint8_t> encryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data);
    static std::vector<uint8_t> decryptASM(std::span<const uint8_t> key,std::span<const uint8_t> data);
    static std::vector<uint8_t> encryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data);
    static std::vector<uint8_t> decryptSYM(std::span<const uint8_t> iv,std::span<const uint8_t> key,std::span<const uint8_t> data);
};
}
