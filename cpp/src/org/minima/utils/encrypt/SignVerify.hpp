#pragma once
#include "GenerateKey.hpp"
namespace minima {
class SignVerify {
public:
    static void main(const std::vector<std::string>* args);
    static constexpr const char* SIGN_ALGO="SHA256withRSA";
    static std::vector<uint8_t> sign(std::span<const uint8_t> key,std::span<const uint8_t> message);
    static bool verify(std::span<const uint8_t> key,std::span<const uint8_t> message,std::span<const uint8_t> signature);
};
}
