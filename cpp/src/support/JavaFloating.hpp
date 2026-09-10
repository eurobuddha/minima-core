#pragma once
#include <algorithm>
#include <charconv>
#include <cmath>
#include <string>
#include <stdexcept>
namespace minima {
template<class Float> std::string javaFloating(Float item) {
    if(std::isnan(item))return "NaN";
    if(std::isinf(item))return std::signbit(item)?"-Infinity":"Infinity";
            if(item==0) return std::signbit(item)?"-0.0":"0.0";
            char buffer[64];auto [end,ec]=std::to_chars(buffer,buffer+64,std::abs(item),std::chars_format::scientific);
            if(ec!=std::errc{}) throw std::runtime_error("JSON double conversion failed");
            std::string scientific(buffer,end);auto e=scientific.find('e');int exponent=std::stoi(scientific.substr(e+1));
            std::string digits=scientific.substr(0,e);digits.erase(std::remove(digits.begin(),digits.end(),'.'),digits.end());
            std::string result;
            if(exponent>=-3&&exponent<7) {
                int point=exponent+1;
                if(point<=0) result="0."+std::string(size_t(-point),'0')+digits;
                else if(point>=int(digits.size())) result=digits+std::string(size_t(point-int(digits.size())),'0')+".0";
                else result=digits.substr(0,size_t(point))+"."+digits.substr(size_t(point));
            } else {
                if(digits.size()==1) {
                    auto rounded=std::to_chars(buffer,buffer+64,std::abs(item),std::chars_format::scientific,1);
                    if(rounded.ec!=std::errc{}) throw std::runtime_error("JSON double conversion failed");
                    scientific.assign(buffer,rounded.ptr);e=scientific.find('e');exponent=std::stoi(scientific.substr(e+1));digits=scientific.substr(0,e);digits.erase(std::remove(digits.begin(),digits.end(),'.'),digits.end());
                }
                result=digits.substr(0,1)+"."+digits.substr(1)+"E"+std::to_string(exponent);
            }
            return std::signbit(item)?"-"+result:result;
}
}
