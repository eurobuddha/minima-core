#pragma once
#include "JavaUnicode.hpp"
#include <unicode/uchar.h>
namespace minima {
// Extracted from InetSocketAddress::equalityKey: UTF-16 length plus folded code points.
inline std::string javaStringIgnoreCaseKey(const std::string& value){
    auto text=unicode::decodeUTF8(value);std::string result=std::to_string(text.size())+":";
    for(size_t i=0;i<text.size();++i){UChar32 c=text[i];if(c>=0xd800&&c<=0xdbff&&i+1<text.size()&&text[i+1]>=0xdc00&&text[i+1]<=0xdfff)c=0x10000+((c-0xd800)<<10)+(text[++i]-0xdc00);result+=std::to_string(u_tolower(u_toupper(c)));result+=',';}
    return result;
}
inline bool javaStringEqualsIgnoreCase(const std::string& left,const std::string& right){return javaStringIgnoreCaseKey(left)==javaStringIgnoreCaseKey(right);}
}
