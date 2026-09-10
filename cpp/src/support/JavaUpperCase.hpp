#pragma once
#include "JavaUnicode.hpp"
#include <unicode/ustring.h>
namespace minima {
inline std::string javaUpperCase(const std::string& input,const std::string& language={}) {
    auto text=unicode::decodeUTF8(input);if(text.size()>INT32_MAX)throw std::length_error("String too long");
    UErrorCode error=U_ZERO_ERROR;auto locale=language.empty()?nullptr:language.c_str();
    auto length=u_strToUpper(nullptr,0,reinterpret_cast<const UChar*>(text.data()),int32_t(text.size()),locale,&error);
    if(error!=U_BUFFER_OVERFLOW_ERROR&&U_FAILURE(error))throw std::runtime_error(u_errorName(error));error=U_ZERO_ERROR;
    std::u16string result(size_t(length),u'\0');u_strToUpper(reinterpret_cast<UChar*>(result.data()),length,reinterpret_cast<const UChar*>(text.data()),int32_t(text.size()),locale,&error);
    if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));return unicode::encodeUTF8(result);
}
}
