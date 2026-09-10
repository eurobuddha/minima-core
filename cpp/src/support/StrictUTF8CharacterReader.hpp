#pragma once
#include "JavaUnicode.hpp"
#include "UTF8Spans.hpp"
#include "ByteIO.hpp"
#include <array>
#include <functional>
namespace minima {
// StreamDecoder over a wrapped ReadableByteChannel, as used by Files.lines.
// Decode the complete fetched byte buffer before publishing any characters.
// An incomplete final prefix is retained until the next channel read.
class StrictUTF8CharacterReader {
    std::function<int32_t(std::span<uint8_t>)> mRead;
    std::string mPending;
    std::u16string mCharacters;
    size_t mPosition=0;
    bool fill() {
        for(;;) {
            std::array<uint8_t,8192> data{};
            auto count=mRead(std::span(data).first(data.size()-mPending.size()));
            auto bytes=mPending;
            if(count>0)bytes.append(reinterpret_cast<const char*>(data.data()),size_t(count));
            std::string complete,pending;
            auto end=reinterpret_cast<const uint8_t*>(bytes.data()+bytes.size());
            unicode::forEachUTF8Span(std::span(reinterpret_cast<const uint8_t*>(bytes.data()),bytes.size()),[&](auto part,bool valid){
                if(valid)complete.append(reinterpret_cast<const char*>(part.data()),part.size());
                else {
                    auto first=part.front();size_t width=first>=0xc2&&first<=0xdf?2:first>=0xe0&&first<=0xef?3:first>=0xf0&&first<=0xf4?4:0;
                    if(count>=0&&width>part.size()&&part.data()+part.size()==end)pending.assign(reinterpret_cast<const char*>(part.data()),part.size());
                    else throw wire::TypedIOException("java.nio.charset.MalformedInputException","Input length = "+std::to_string(part.size()));
                }
            });
            mPending=std::move(pending);mCharacters=unicode::decodeUTF8(complete);mPosition=0;
            if(!mCharacters.empty())return true;
            if(count<0)return false;
        }
    }
public:
    explicit StrictUTF8CharacterReader(std::function<int32_t(std::span<uint8_t>)> read):mRead(std::move(read)){}
    int read(){if(mPosition==mCharacters.size()&&!fill())return -1;return mCharacters[mPosition++];}
};
}
