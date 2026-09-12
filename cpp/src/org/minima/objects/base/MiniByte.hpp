#pragma once
#include "org/minima/utils/Streamable.hpp"
// Adapted from minima-core-cpp/src/objects/MiniByte.hpp; see REUSED_CODE_LICENSE.
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace minima {
class MiniByte:public Streamable {
public:
    static MiniByte FALSE;
    static MiniByte TRUE;
    MiniByte();
    explicit MiniByte(int value);
    explicit MiniByte(bool value);
    virtual int getValue() const;
    virtual std::int8_t getByteValue() const;
    virtual bool isEqual(const MiniByte& other) const;
    virtual bool isFalse() const;
    virtual bool isTrue() const;
    virtual std::string toString() const;
    static void WriteToStream(std::vector<uint8_t>& output,bool value) {MiniByte(value).writeDataStream(output);}
    static MiniByte ReadFromStream(std::span<const uint8_t> data,size_t& offset) {return deserialise(data,offset);}
    void writeDataStream(std::vector<uint8_t>& output) const override;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    std::vector<std::uint8_t> serialise() const;
    static MiniByte deserialise(std::span<const std::uint8_t> data, std::size_t& offset);
private:
    std::uint8_t m_value{0};
};
}
