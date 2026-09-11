// Java counterpart: src/org/minima/objects/base/MiniByte.java
#include "MiniByte.hpp"
#include "support/ByteIO.hpp"
#include <bit>
#include <stdexcept>

namespace minima {
const MiniByte MiniByte::FALSE(0);
const MiniByte MiniByte::TRUE(1);
MiniByte::MiniByte() = default;
MiniByte::MiniByte(int value) : m_value(static_cast<std::uint8_t>(value)) {}
MiniByte::MiniByte(bool value) : m_value(value ? 1 : 0) {}
int MiniByte::getValue() const { return m_value; }
std::int8_t MiniByte::getByteValue() const { return std::bit_cast<std::int8_t>(m_value); }
bool MiniByte::isEqual(const MiniByte& other) const { return m_value == other.m_value; }
bool MiniByte::isFalse() const { return m_value == 0; }
bool MiniByte::isTrue() const { return !isFalse(); }
std::string MiniByte::toString() const { return std::to_string(getValue()); }
void MiniByte::writeDataStream(std::vector<uint8_t>& output) const {output.push_back(m_value);}
void MiniByte::readDataStream(std::span<const uint8_t> data,size_t& offset) {m_value=wire::byte(data,offset);}
std::vector<std::uint8_t> MiniByte::serialise() const {return Streamable::serialise();}
MiniByte MiniByte::deserialise(std::span<const std::uint8_t> data, std::size_t& offset) {
    MiniByte value;value.readDataStream(data,offset);return value;
}
}
