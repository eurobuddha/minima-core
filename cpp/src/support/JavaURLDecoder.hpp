#pragma once
#include "JavaUnicode.hpp"
#include <stdexcept>
namespace minima {
// URLDecoder.decode(text,"UTF-8"), using the tested JDK 26 strict hex alphabet.
inline std::string javaURLDecode(const std::string& input){
    auto text=unicode::decodeUTF8(input);std::u16string result;
    auto hex=[](char16_t c)->int{if(c>=u'0'&&c<=u'9')return c-u'0';if(c>=u'A'&&c<=u'F')return c-u'A'+10;if(c>=u'a'&&c<=u'f')return c-u'a'+10;throw std::invalid_argument("URLDecoder: Illegal hex characters in escape (%) pattern - not a hexadecimal digit: \""+unicode::encodeUTF8(std::u16string(1,c))+"\" = "+std::to_string(uint16_t(c)));};
    for(size_t i=0;i<text.size();){
        if(text[i]==u'+'){result+=u' ';++i;}
        else if(text[i]==u'%'){
            std::string bytes;
            while(i<text.size()&&text[i]==u'%'){
                if(text.size()-i<3)throw std::invalid_argument("URLDecoder: Incomplete trailing escape (%) pattern");
                int high=hex(text[i+1]);int low=hex(text[i+2]);bytes+=char((high<<4)|low);i+=3;
            }
            result+=unicode::decodeUTF8(bytes);
        }else result+=text[i++];
    }
    return unicode::encodeUTF8(result);
}
}
