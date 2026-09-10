#pragma once
#include <cstdint>
#include <span>
#include <cstddef>
namespace minima::unicode {
// Extracted from MiniString: preserve Java malformed-prefix consumption.
template<class Consumer> void forEachUTF8Span(std::span<const uint8_t> bytes,Consumer&& consume) {
    for (size_t i = 0; i < bytes.size();) {
        uint8_t first = bytes[i];
        if (first < 0x80) { consume(bytes.subspan(i,1),true); ++i; continue; }
        int width = first >= 0xC2 && first <= 0xDF ? 2 :
                    first >= 0xE0 && first <= 0xEF ? 3 :
                    first >= 0xF0 && first <= 0xF4 ? 4 : 0;
        size_t consumed = 1;
        bool valid = width != 0;
        while (valid && consumed < static_cast<size_t>(width)) {
            if (i + consumed == bytes.size()) { valid = false; break; }
            uint8_t next = bytes[i + consumed];
            if ((next & 0xC0) != 0x80 ||
                (consumed == 1 && ((first == 0xE0 && next < 0xA0) ||
                                   (first == 0xF0 && next < 0x90) ||
                                   (first == 0xF4 && next > 0x8F)))) {
                valid = false;
                break;
            }
            ++consumed;
        }
        // A UTF-8 encoded surrogate consumes the complete three-byte sequence
        // before Java replaces it, unlike an overlong prefix.
        if (valid && width == 3 && first == 0xED && bytes[i+1] >= 0xA0) valid = false;
        consume(bytes.subspan(i,consumed),valid);
        i += consumed;
    }
}
}
