#include "JSONWriter.hpp"
namespace minima::json {
void JSONWriter::write(const std::string& value) { buffer += value; }
void JSONWriter::write(int value) {
    unsigned c = static_cast<unsigned>(value) & 0xFFFF;
    if (c < 0x80) buffer += static_cast<char>(c);
    else if (c < 0x800) { buffer += static_cast<char>(0xC0|(c>>6)); buffer += static_cast<char>(0x80|(c&63)); }
    else { buffer += static_cast<char>(0xE0|(c>>12)); buffer += static_cast<char>(0x80|((c>>6)&63)); buffer += static_cast<char>(0x80|(c&63)); }
}
const std::string& JSONWriter::getFullString() const { return buffer; }
const std::string& JSONWriter::toString() const { return buffer; }
}
