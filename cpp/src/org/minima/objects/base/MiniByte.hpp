#pragma once
// Adapted from minima-core-cpp/src/objects/MiniByte.hpp; see REUSED_CODE_LICENSE.
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace minima {
class MiniByte {
public:
    static const MiniByte FALSE;
    static const MiniByte TRUE;
    MiniByte();
    explicit MiniByte(int value);
    explicit MiniByte(bool value);
    int getValue() const;
    std::int8_t getByteValue() const;
    bool isEqual(const MiniByte& other) const;
    bool isFalse() const;
    bool isTrue() const;
    std::string toString() const;
    std::vector<std::uint8_t> serialise() const;
    static MiniByte deserialise(std::span<const std::uint8_t> data, std::size_t& offset);
private:
    std::uint8_t m_value{0};
};
}
