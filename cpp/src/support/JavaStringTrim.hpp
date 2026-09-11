#pragma once
#include <cstdint>
#include <string>
namespace minima {
// Extracted unchanged from BIP39's Java String.trim translation.
inline std::string javaStringTrim(const std::string& text) {
    size_t start=0,end=text.size();while(start<end&&uint8_t(text[start])<=32)++start;while(end>start&&uint8_t(text[end-1])<=32)--end;return text.substr(start,end-start);
}
inline std::u16string javaStringTrim(const std::u16string& text) {
    size_t start=0,end=text.size();while(start<end&&text[start]<=32)++start;while(end>start&&text[end-1]<=32)--end;return text.substr(start,end-start);
}
}
