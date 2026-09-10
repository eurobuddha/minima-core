#pragma once
#include "org/minima/objects/Token.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands::send_detail {
// StringTokenizer(String, ":") ignores empty fields and leaves extra fields unread.
inline std::string nextRecipientToken(const std::string& input,size_t& offset){
    offset=input.find_first_not_of(':',offset);
    if(offset==std::string::npos)throw JavaTypedException("java.util.NoSuchElementException","");
    auto end=input.find(':',offset);auto value=input.substr(offset,end==std::string::npos?end:end-offset);
    offset=end==std::string::npos?input.size():end;return value;
}
inline const Token& requiredToken(const std::optional<Token>& token){if(!token)throw JavaNullPointerException("null Token");return *token;}
}
