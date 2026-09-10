#pragma once
#include "ByteIO.hpp"
namespace minima::wire {
class SocketException:public TypedIOException {
protected:
    SocketException(std::string name,const std::string& message):TypedIOException(std::move(name),message){}
public:
    explicit SocketException(const std::string& message):SocketException("java.net.SocketException",message){}
};
class BindException:public SocketException {
public:
    explicit BindException(const std::string& message):SocketException("java.net.BindException",message){}
};
}
