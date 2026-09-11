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

void MiniString::writeDataStream(std::vector<uint8_t>& output) const {MiniData(getData()).writeDataStream(output);}
std::vector<uint8_t> MiniString::serialise() const {return Streamable::serialise();}
void MiniString::readDataStream(std::span<const uint8_t> data,size_t& offset){auto value=deserialise(data,offset);m_value=std::move(value.m_value);}

MiniString MiniString::deserialise(std::span<const uint8_t> data, size_t& offset) {return MiniString(MiniData::deserialise(data,offset).bytes());}

} // namespace minima
