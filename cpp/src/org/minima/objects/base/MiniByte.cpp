// Java counterpart: src/org/minima/objects/base/MiniByte.java
#include "MiniByte.hpp"
#include "support/ByteIO.hpp"
#include <bit>
#include <stdexcept>

namespace minima {
MiniByte MiniByte::FALSE(0);
MiniByte MiniByte::TRUE(1);
MiniByte::MiniByte() = default;
MiniByte::MiniByte(int value) : m_value(static_cast<std::uint8_t>(value)) {}
MiniByte::MiniByte(bool value) : m_value(value ? 1 : 0) {}
int MiniByte::getValue() const { return m_value; }
std::int8_t MiniByte::getByteValue() const { return std::bit_cast<std::int8_t>(m_value); }
bool MiniByte::isEqual(const MiniByte& other) const {
    // Java evaluates the left getter before the right, including side effects
    // and exceptions from either override. C++ == does not impose that order.
    const auto value = getValue();
    return value == other.getValue();
}
bool MiniByte::isFalse() const { return isEqual(FALSE); }
bool MiniByte::isTrue() const { return !isFalse(); }
std::string MiniByte::toString() const { return std::to_string(getValue()); }
void MiniByte::writeDataStream(std::vector<uint8_t>& output) const {output.push_back(m_value);}
void MiniByte::readDataStream(std::span<const uint8_t> data,size_t& offset) {m_value=wire::byte(data,offset);}
std::vector<std::uint8_t> MiniByte::serialise() const {return Streamable::serialise();}
MiniByte MiniByte::deserialise(std::span<const std::uint8_t> data, std::size_t& offset) {
    MiniByte value;value.readDataStream(data,offset);return value;
}
}
