#include "MiniString.hpp"
#include "MiniData.hpp"
#include "support/UTF8Spans.hpp"
#include <stdexcept>

namespace minima {

MiniString::MiniString(const std::string& s)
    : m_value(normalizeUTF8({reinterpret_cast<const uint8_t*>(s.data()), s.size()})) {}
MiniString::MiniString(std::span<const uint8_t> bytes) : m_value(normalizeUTF8(bytes)) {}

std::string MiniString::normalizeUTF8(std::span<const uint8_t> bytes) {
    std::string out;
    unicode::forEachUTF8Span(bytes,[&](std::span<const uint8_t> part,bool valid){
        if(valid)out.append(reinterpret_cast<const char*>(part.data()),part.size());
        else out += "\xEF\xBF\xBD";
    });
    return out;
}

MiniString MiniString::concat(const MiniString& rhs) const {
    return MiniString(m_value + rhs.m_value);
}

MiniString MiniString::subset(size_t start, size_t len) const {
    if (start > m_value.size() || len > m_value.size() - start)
        throw std::out_of_range("MiniString::subset out of range");
    return MiniString(m_value.substr(start, len));
}

std::vector<uint8_t> MiniString::serialise() const {
    // Java: MiniData(getData()).writeDataStream(out)
    // MiniData.writeDataStream: [4-byte BE int: length][raw bytes]
    if (m_value.size() > INT32_MAX)
        throw std::runtime_error("MiniString too long");
    uint32_t len = static_cast<uint32_t>(m_value.size());
    std::vector<uint8_t> out;
    out.push_back((len >> 24) & 0xFF);
    out.push_back((len >> 16) & 0xFF);
    out.push_back((len >>  8) & 0xFF);
    out.push_back( len        & 0xFF);
    for (char c : m_value) out.push_back(static_cast<uint8_t>(c));
    return out;
}

MiniString MiniString::deserialise(std::span<const uint8_t> data, size_t& offset) {return MiniString(MiniData::deserialise(data,offset).bytes());}

} // namespace minima
