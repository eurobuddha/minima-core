#pragma once
#include "JavaUnicode.hpp"
#include "JavaFinalSigma.hpp"
#include <unicode/ustring.h>
namespace minima {
inline std::u16string javaLowerCaseUTF16(std::u16string text,const std::string& language={}) {
    if(text.size()>INT32_MAX)throw std::length_error("String too long");
    unicode::applyJavaFinalSigma(text);
    UErrorCode error=U_ZERO_ERROR;auto locale=language.empty()?nullptr:language.c_str();
    auto length=u_strToLower(nullptr,0,reinterpret_cast<const UChar*>(text.data()),int32_t(text.size()),locale,&error);
    if(error!=U_BUFFER_OVERFLOW_ERROR&&U_FAILURE(error))throw std::runtime_error(u_errorName(error));error=U_ZERO_ERROR;
    std::u16string result(size_t(length),u'\0');u_strToLower(reinterpret_cast<UChar*>(result.data()),length,reinterpret_cast<const UChar*>(text.data()),int32_t(text.size()),locale,&error);
    if(U_FAILURE(error))throw std::runtime_error(u_errorName(error));return result;
}
inline std::string javaLowerCase(const std::string& name,const std::string& language={}) {return unicode::encodeUTF8(javaLowerCaseUTF16(unicode::decodeUTF8(name),language));}
}
