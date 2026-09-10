#pragma once
#include "JavaUnicode.hpp"
#include "JavaExceptions.hpp"
#include <unicode/uchar.h>
#include <cstdint>
#include <limits>
namespace minima {
// Decimal Integer.parseInt(String): BMP Unicode digits, optional ASCII sign,
// no whitespace or exponent, and checked signed-32-bit bounds.
template<class Integer> inline Integer javaParseSignedInteger(const std::string& text){
    auto units=unicode::decodeUTF8(text);size_t index=0;bool negative=false;
    if(!units.empty()&&(units[0]==u'-'||units[0]==u'+')){negative=units[0]==u'-';++index;}
    auto fail=[&](){throw JavaNumberFormatException("For input string: \""+text+"\"");};
    if(index==units.size())fail();uint64_t value=0,limit=negative?uint64_t(std::numeric_limits<Integer>::max())+1:uint64_t(std::numeric_limits<Integer>::max());
    for(;index<units.size();++index){int digit=u_digit(units[index],10);if(digit<0||value>(limit-uint64_t(digit))/10)fail();value=value*10+uint64_t(digit);}
    if(negative&&value==uint64_t(std::numeric_limits<Integer>::max())+1)return std::numeric_limits<Integer>::min();
    return static_cast<Integer>(negative?-int64_t(value):int64_t(value));
}
inline int32_t javaParseInt(const std::string& text){return javaParseSignedInteger<int32_t>(text);}
inline int64_t javaParseLong(const std::string& text){return javaParseSignedInteger<int64_t>(text);}
}
