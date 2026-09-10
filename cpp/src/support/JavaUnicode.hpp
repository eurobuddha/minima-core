#pragma once
#include "org/minima/objects/base/MiniString.hpp"
namespace minima::unicode {
inline std::u16string decodeUTF8(const std::string& input) {
    auto text=MiniString(input).str();std::u16string out;
    for(size_t i=0;i<text.size();) {
        auto first=uint8_t(text[i++]);unsigned count=first<128?1:first<224?2:first<240?3:4;
        uint32_t value=first&(count==1?127:count==2?31:count==3?15:7);
        for(unsigned j=1;j<count;++j) value=(value<<6)|(uint8_t(text[i++])&63);
        if(value<65536) out.push_back(char16_t(value));else {value-=65536;out.push_back(char16_t(0xd800+(value>>10)));out.push_back(char16_t(0xdc00+(value&1023)));}
    }
    return out;
}
inline std::string encodeUTF8(const std::u16string& text) {
    std::string out;
    for(size_t i=0;i<text.size();++i) {
        uint32_t value=text[i];
        if(value>=0xd800&&value<=0xdbff&&i+1<text.size()&&text[i+1]>=0xdc00&&text[i+1]<=0xdfff) {value=65536+((value-0xd800)<<10)+(text[++i]-0xdc00);}
        else if(value>=0xd800&&value<=0xdfff) value='?'; // Java UTF-8 encoder's replacement byte.
        if(value<128) out+=char(value);
        else if(value<2048) {out+=char(0xc0|(value>>6));out+=char(0x80|(value&63));}
        else if(value<65536) {out+=char(0xe0|(value>>12));out+=char(0x80|((value>>6)&63));out+=char(0x80|(value&63));}
        else {out+=char(0xf0|(value>>18));out+=char(0x80|((value>>12)&63));out+=char(0x80|((value>>6)&63));out+=char(0x80|(value&63));}
    }
    return out;
}
}
