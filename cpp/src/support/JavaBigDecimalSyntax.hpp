#pragma once
#include "JavaExceptions.hpp"
#include "JavaUnicode.hpp"
#include <unicode/uchar.h>
namespace minima {
// BigDecimal(char[], ..., MathContext) syntax and diagnostic order, from the
// local OpenJDK 26 source. Arithmetic/rounding remain in MiniNumber.
inline std::string javaBigDecimalSyntax(const std::string& text) {
    auto units=unicode::decodeUTF8(text);
    auto missing=[](){throw JavaTypedException("java.lang.NumberFormatException","");};
    if(units.empty()) missing();
    size_t at=0;
    std::string normalized;
    if(units[at]=='+'||units[at]=='-') normalized+=char(units[at++]);
    bool compact=units.size()-at<=18,dot=false,haveDigit=false;
    for(;at<units.size();++at) {
        auto c=units[at];int digit=u_digit(c,10);
        if(digit>=0) {haveDigit=true;normalized+=char('0'+digit);continue;}
        if(c=='.') {
            if(dot) throw JavaNumberFormatException("Character array contains more than one decimal point.");
            dot=true;normalized+='.';continue;
        }
        if(c=='e'||c=='E') {
            normalized+=char(c);
            if(++at==units.size()) missing();
            if(units[at]=='+'||units[at]=='-') {
                normalized+=char(units[at++]);if(at==units.size()) missing();
            }
            // parseExp ignores redundant leading zeros before its length check.
            while(units.size()-at>10&&u_digit(units[at],10)==0) {normalized+='0';++at;}
            if(units.size()-at>10) throw JavaNumberFormatException("Too many nonzero exponent digits.");
            for(;at<units.size();++at) {
                digit=u_digit(units[at],10);
                if(digit<0) throw JavaNumberFormatException("Not a digit.");
                normalized+=char('0'+digit);
            }
            break;
        }
        if(!compact) throw JavaNumberFormatException("Character array is missing \"e\" notation exponential mark.");
        throw JavaNumberFormatException("Character "+unicode::encodeUTF8(std::u16string(1,c))+
            " is neither a decimal digit number, decimal point, nor \"e\" notation exponential mark.");
    }
    if(!haveDigit) throw JavaNumberFormatException("No digits found.");
    return normalized;
}
}
