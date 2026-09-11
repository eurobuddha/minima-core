#include "BaseConverter.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include <bit>
#include <iostream>
#include <algorithm>
#include "support/JavaExceptions.hpp"
#include "support/JavaLowerCase.hpp"
#include <unicode/uchar.h>
namespace minima {
std::string BaseConverter::encode16(std::span<const uint8_t> data) {
    return data.empty() ? "" : MiniData::fromBytes(data.data(), data.size()).to0xString();
}
std::vector<uint8_t> BaseConverter::decode16(const std::string& hex) { return MiniData::fromHex(hex).bytes(); }
std::string BaseConverter::numberToHex(int32_t number) {
    uint32_t v=std::bit_cast<uint32_t>(number);
    std::vector<uint8_t> bytes;
    do { bytes.push_back(uint8_t(v)); v >>= 8; } while(v);
    std::reverse(bytes.begin(),bytes.end());
    return encode16(bytes);
}
int32_t BaseConverter::hexToNumber(const std::string& hex) {
    auto s=hex.substr(2);
    bool negative=false;
    if(!s.empty() && (s[0]=='-' || s[0]=='+')) { negative=s[0]=='-'; s.erase(0,1); }
    if(s.empty()) throw std::invalid_argument("Empty integer");
    uint32_t value=0;
    for(char c:s) {
        int d=c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;
        if(d<0) throw std::invalid_argument("Invalid hex integer");
        value=value*16+uint32_t(d);
    }
    return std::bit_cast<int32_t>(negative?0u-value:value);
}
std::string BaseConverter::encode32(std::span<const uint8_t> data) {
    static constexpr char alphabet[]="0123456789ABCDEFGHWJKYMNZPQRSTUV";
    // Adapt BigInt::toDecimalString's single-byte-divisor loop. Calling the
    // general division routine twice per digit makes larger Mx data very slow.
    std::vector<uint8_t> remaining(data.begin(),data.end());
    std::string out;
    do {
        std::vector<uint8_t> quotient;unsigned remainder=0;
        for(uint8_t byte:remaining){
            unsigned value=remainder*256+byte;
            if(!quotient.empty()||value/32)quotient.push_back(uint8_t(value/32));
            remainder=value%32;
        }
        out.push_back(alphabet[remainder]);remaining=std::move(quotient);
    } while(!remaining.empty());
    std::reverse(out.begin(),out.end());
    return "Mx"+out;
}
std::vector<uint8_t> BaseConverter::decode32(const std::string& encoded) {
    auto s=unicode::decodeUTF8(javaLowerCase(encoded));
    if(s.starts_with(u"mx"))s.erase(0,2);
    for(auto& c:s){if(c==u'w')c=u'i';else if(c==u'y')c=u'l';else if(c==u'z')c=u'o';}
    auto minus=s.rfind(u'-'),plus=s.rfind(u'+');bool negative=minus!=std::u16string::npos;
    if((negative&&(minus!=0||plus!=std::u16string::npos))||(!negative&&plus!=std::u16string::npos&&plus!=0))throw JavaNumberFormatException("Illegal embedded sign character");
    size_t cursor=negative||plus!=std::u16string::npos?1:0;
    if(cursor==s.size())throw JavaNumberFormatException("Zero length BigInteger");
    while(cursor<s.size()&&u_digit(s[cursor],32)==0)++cursor;
    auto firstLength=(s.size()-cursor)%6;if(!firstLength)firstLength=6;
    BigInt value;
    for(size_t length=firstLength;cursor<s.size();length=6){
        auto group=s.substr(cursor,length);cursor+=length;
        for(char16_t c:group){int digit=u_digit(c,32);
            if(digit<0)throw JavaNumberFormatException("For input string: \""+unicode::encodeUTF8(group)+"\" under radix 32");
            value=value*BigInt(32)+BigInt(digit);
        }
    }
    if(negative&&!value.isZero()){
        auto hex=encode16(value.toJavaBytes()).substr(2);auto first=hex.find_first_not_of('0');hex.erase(0,first);
        for(char& c:hex)if(c>='A'&&c<='F')c+=32;
        throw JavaNumberFormatException("Invalid HEX string in decode16 : 0x-"+hex);
    }
    auto bytes=value.toJavaBytes();
    if(bytes.size()>1 && bytes.front()==0) bytes.erase(bytes.begin());
    return bytes;
}
void BaseConverter::main(const std::vector<std::string>*){
    auto hex=MiniData::fromHex("0x000001");auto hstr=hex.to0xString();
    std::cout<<"HEX : "<<hstr.size()<<" "<<hstr<<std::endl;
    auto base32=encode32(hex.bytes());std::cout<<"B32 : "<<base32.size()<<" "<<base32<<std::endl;
    // The source uses the hex-only MiniData(String) constructor here. Preserve
    // its NumberFormatException for Mx1 rather than silently changing the demo.
    auto conv=MiniData::fromHex(base32);auto convstr=conv.to0xString();
    std::cout<<"COV : "<<convstr.size()<<" "<<convstr<<std::endl;
    std::cout<<(conv.isEqual(hex)?"true":"false")<<std::endl;
}

}
