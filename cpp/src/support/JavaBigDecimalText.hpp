#pragma once
#include <cstdint>
#include <string>
namespace minima {
// Extracted from MMREntryNumber::toBigDecimalString. Input is a canonical signed
// unscaled integer; BigDecimal retains its scale and significant trailing zeros.
inline std::string javaBigDecimalText(std::string value, int32_t scale) {
    bool negative=value.front()=='-';if(negative)value.erase(0,1);
    int64_t exponent=int64_t(value.size())-1-scale;
    if(scale>=0 && exponent>=-6) {
        if(scale>0) {if(size_t(scale)>=value.size())value.insert(0,size_t(scale)-value.size()+1,'0');value.insert(value.size()-scale,1,'.');}
    } else {
        if(value.size()>1)value.insert(1,1,'.');
        value+="E";if(exponent>=0)value+="+";value+=std::to_string(exponent);
    }
    return negative?"-"+value:value;
}
}
