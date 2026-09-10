#pragma once
#include "ByteIO.hpp"
#include "JavaUnicode.hpp"
#include <openssl/evp.h>
#include <string_view>
#include <sstream>
namespace minima {
inline std::string encodeBase64(std::span<const uint8_t> data){
    if(data.size()>INT32_MAX)throw std::length_error("Base64 input too large");
    std::string result((data.size()+2)/3*4+1,'\0');auto length=EVP_EncodeBlock(reinterpret_cast<unsigned char*>(result.data()),data.data(),int(data.size()));
    if(length<0)throw std::runtime_error("Base64 encoding failed");result.resize(size_t(length));return result;
}
// JDK Base64.getDecoder(): String input first passes through ISO-8859-1.
inline std::vector<uint8_t> decodeBase64(std::string_view input) {
    auto unicode=unicode::decodeUTF8(std::string(input));std::string text;
    for(size_t i=0;i<unicode.size();++i){auto c=unicode[i];if(c<=255)text+=char(c);else{text+='?';if(c>=0xd800&&c<=0xdbff&&i+1<unicode.size()&&unicode[i+1]>=0xdc00&&unicode[i+1]<=0xdfff)++i;}}
    constexpr std::string_view alphabet="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if(text.size()==1)throw std::invalid_argument("Input byte[] should at least have 2 bytes for base64 bytes");
    std::vector<uint8_t> result;uint32_t accumulator=0;unsigned bits=0;size_t digits=0;
    for(size_t i=0;i<text.size();++i){
        if(text[i]=='='){
            auto remaining=digits%4;size_t consumed=i+1;
            if(remaining==0||(remaining==2&&(consumed==text.size()||text[consumed++]!='=')))throw std::invalid_argument("Input byte array has wrong 4-byte ending unit");
            if(remaining==1)throw std::invalid_argument("Last unit does not have enough valid bits");
            if(consumed<text.size())throw std::invalid_argument("Input byte array has incorrect ending byte at "+std::to_string(consumed));
            return result;
        }
        auto digit=alphabet.find(text[i]);if(digit==std::string_view::npos){int c=int8_t(text[i]);std::ostringstream number;if(c<0){number<<'-';c=-c;}number<<std::hex<<c;throw std::invalid_argument("Illegal base64 character "+number.str());}
        accumulator=(accumulator<<6)|uint32_t(digit);bits+=6;++digits;
        if(bits>=8){bits-=8;result.push_back(uint8_t(accumulator>>bits));}
    }
    if(digits%4==1)throw std::invalid_argument("Last unit does not have enough valid bits");return result;
}
}
