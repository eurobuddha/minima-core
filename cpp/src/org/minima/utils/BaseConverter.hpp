#pragma once
#include "org/minima/objects/base/MiniData.hpp"
namespace minima {
class BaseConverter {
public:
    static void main(const std::vector<std::string>* args=nullptr);
    static std::string encode16(std::span<const uint8_t> data);
    static std::vector<uint8_t> decode16(const std::string& hex);
    static std::string numberToHex(int32_t number);
    static int32_t hexToNumber(const std::string& hex);
    static std::string encode32(std::span<const uint8_t> data);
    static std::vector<uint8_t> decode32(const std::string& encoded);
};
}
