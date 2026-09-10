#pragma once
#include "JavaExceptions.hpp"
#include "JavaStringTrim.hpp"
#include <cstdlib>
#include <regex>
#include <limits>
namespace minima {
// The JSON lexer already uses strtod after validating its grammar. Java's public
// floating parser additionally permits signs, type suffixes, hex, NaN/Infinity.
inline double javaParseDouble(const std::string& source){
    auto text=javaStringTrim(source);
    static const std::regex syntax(R"([+-]?(NaN|Infinity|(([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)?|0[xX]([0-9a-fA-F]+(\.[0-9a-fA-F]*)?|\.[0-9a-fA-F]+)[pP][+-]?[0-9]+)[fFdD]?))");
    if(!std::regex_match(text,syntax))throw JavaTypedException("java.lang.NumberFormatException",text.empty()?"empty String":"For input string: \""+text+"\"");
    if(text.ends_with("NaN"))return std::numeric_limits<double>::quiet_NaN();
    if(text.ends_with('f')||text.ends_with('F')||text.ends_with('d')||text.ends_with('D'))text.pop_back();
    char* end=nullptr;auto result=std::strtod(text.c_str(),&end);
    if(end!=text.c_str()+text.size())throw JavaTypedException("java.lang.NumberFormatException","For input string: \""+text+"\"");return result;
}
}
