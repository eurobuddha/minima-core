#pragma once
#include "NIOConnections.hpp"
#include "JavaUnicode.hpp"
#include <array>
#include <functional>
namespace minima {
// Buffered UTF-8 socket input with Java Reader UTF-16 counts and readLine rules.
// Byte framing uses the tested MiniString malformed-prefix consumption rules;
// actual decoding and surrogate generation remain in JavaUnicode/MiniString.
class SocketCharacterReader {
    std::function<int32_t(std::span<uint8_t>)> mReadBytes;
    std::function<int()> mReadCharacter;
    std::array<uint8_t,8192> mBytes{};
    size_t mPosition=0,mEnd=0;
    bool mEOF=false,mSkipLF=false;
    std::optional<char16_t> mPending;
    int peekByte(){
        if(mPosition==mEnd&&!mEOF){auto count=mReadBytes(mBytes);mPosition=0;mEOF=count<0;mEnd=mEOF?0:size_t(count);}
        return mPosition==mEnd?-1:mBytes[mPosition];
    }
    int rawCharacter(){
        if(mReadCharacter)return mReadCharacter();
        if(mPending){int value=*mPending;mPending.reset();return value;}
        int initial=peekByte();if(initial<0)return -1;++mPosition;uint8_t first=uint8_t(initial);
        std::string bytes(1,char(first));int width=first>=0xc2&&first<=0xdf?2:first>=0xe0&&first<=0xef?3:first>=0xf0&&first<=0xf4?4:1;
        while(bytes.size()<size_t(width)){
            int next=peekByte();if(next<0||(next&0xc0)!=0x80||
                (bytes.size()==1&&((first==0xe0&&next<0xa0)||(first==0xf0&&next<0x90)||(first==0xf4&&next>0x8f))))break;
            bytes+=char(next);++mPosition;
        }
        auto characters=unicode::decodeUTF8(bytes);if(characters.size()>1)mPending=characters[1];return characters[0];
    }
public:
    explicit SocketCharacterReader(std::shared_ptr<PosixNIOSocket> socket)
        :SocketCharacterReader([socket=std::move(socket)](std::span<uint8_t> bytes){return nioReadSome(socket,bytes);}){}
    explicit SocketCharacterReader(std::function<int32_t(std::span<uint8_t>)> readBytes):mReadBytes(std::move(readBytes)){}
    explicit SocketCharacterReader(std::function<int()> readCharacter):mReadCharacter(std::move(readCharacter)){}
    int read(){int value=rawCharacter();if(mSkipLF){mSkipLF=false;if(value=='\n')value=rawCharacter();}return value;}
    // A terminal EOF is temporary; callers that own a reusable input stream
    // can retry it on the next readLine. Socket consumers retain sticky EOF.
    std::optional<std::u16string> readLineUTF16(bool retryEOF=false){
        if(retryEOF)mEOF=false;
        std::u16string line;int value;
        while((value=read())>=0){if(value=='\n')break;if(value=='\r'){mSkipLF=true;break;}line+=char16_t(value);}
        if(value<0&&line.empty())return std::nullopt;return line;
    }
    std::optional<std::string> readLine(){auto line=readLineUTF16();return line?std::optional(unicode::encodeUTF8(*line)):std::nullopt;}
    // A short read at EOF leaves the rest of the source's char[] zero-filled.
    std::pair<std::u16string,size_t> readCharacters(size_t count){
        std::u16string characters(count,u'\0');size_t total=0;while(total<count){int value=read();if(value<0)break;characters[total++]=char16_t(value);}return {std::move(characters),total};
    }
};
}
