#pragma once
#include "ByteIO.hpp"
#include <string>
namespace minima {
// Shared SQL codec logging; these readers throw Java EOFException without a message.
inline std::string javaIOExceptionText(const wire::IOException& error) {
    if(auto typed=dynamic_cast<const wire::TypedIOException*>(&error))return typed->className()+": "+error.message();
    if(auto eof=dynamic_cast<const wire::EOFException*>(&error))return std::string("java.io.EOFException")+(eof->preservesMessage()?": "+eof->message():"");
    return std::string("java.io.IOException: ") + error.message();
}
}
