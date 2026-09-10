#pragma once
#include "support/NativeStackTrace.hpp"
#include <stdexcept>
#include <string>
#include <utility>
namespace minima::json::parser {
class ParseException : public std::exception, public NativeStackTrace {
    int errorType,position;
    std::string unexpectedObject;
    mutable std::string message;
public:
    static constexpr int ERROR_UNEXPECTED_CHAR=0,ERROR_UNEXPECTED_TOKEN=1,ERROR_UNEXPECTED_EXCEPTION=2;
    explicit ParseException(int type,std::string unexpected="null"):ParseException(-1,type,std::move(unexpected)){}
    ParseException(int pos,int type,std::string unexpected):errorType(type),position(pos),unexpectedObject(std::move(unexpected)){}
    int getErrorType() const {return errorType;}
    void setErrorType(int type) {errorType=type;}
    int getPosition() const {return position;}
    void setPosition(int pos) {position=pos;}
    const std::string& getUnexpectedObject() const {return unexpectedObject;}
    void setUnexpectedObject(std::string object) {unexpectedObject=std::move(object);}
    std::string getMessage() const;
    const char* what() const noexcept override;
};
}
