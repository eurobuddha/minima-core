#include "ParseException.hpp"
namespace minima::json::parser {
std::string ParseException::getMessage() const {
    auto pos=std::to_string(position);
    switch(errorType) {
        case ERROR_UNEXPECTED_CHAR:return "Unexpected character ("+unexpectedObject+") at position "+pos+".";
        case ERROR_UNEXPECTED_TOKEN:return "Unexpected token "+unexpectedObject+" at position "+pos+".";
        case ERROR_UNEXPECTED_EXCEPTION:return "Unexpected exception at position "+pos+": "+unexpectedObject;
        default:return "Unkown error at position "+pos+".";
    }
}
const char* ParseException::what() const noexcept {try {message=getMessage();return message.c_str();}catch(...) {return "JSON parse error";}}
}
