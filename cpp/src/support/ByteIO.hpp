#pragma once
// Vector-based equivalent of the inspected DataStream writeBytes composition.
#include "NativeStackTrace.hpp"
#include <cstdint>
#include <span>
#include <vector>
#include <stdexcept>
#include <string>
#include <utility>
namespace minima::wire {
class IOException : public std::runtime_error, public NativeStackTrace {
    std::string mMessage;
public:
    explicit IOException(const std::string& message):std::runtime_error(message),mMessage(message){}
    explicit IOException(const char* message):IOException(std::string(message)){}
    const std::string& message() const noexcept{return mMessage;}
};
class TypedIOException : public IOException {
    std::string mClass;
public:
    TypedIOException(std::string name,const std::string& message):IOException(message),mClass(std::move(name)){}
    const std::string& className() const{return mClass;}
};
class EOFException : public IOException {
    bool mPreserveMessage=false;
public:
    using IOException::IOException;
    EOFException(const std::string& message,bool preserveMessage):IOException(message),mPreserveMessage(preserveMessage){}
    bool preservesMessage() const noexcept{return mPreserveMessage;}
};
using Bytes = std::vector<uint8_t>;
inline void append(Bytes& to, std::span<const uint8_t> from) { to.insert(to.end(), from.begin(), from.end()); }
template<class... T> Bytes join(const T&... objects) {
    Bytes out;
    (append(out, objects.serialise()), ...);
    return out;
}
inline uint8_t byte(std::span<const uint8_t> data, size_t& offset) {
    if (offset >= data.size()) throw EOFException("Truncated byte");
    return data[offset++];
}
inline uint32_t integer(std::span<const uint8_t> data,size_t& offset) {uint32_t value=0;for(int i=0;i<4;++i)value=(value<<8)|byte(data,offset);return value;}
inline std::span<const uint8_t> readFully(std::span<const uint8_t> data,size_t& offset,size_t count) {
    if(offset>data.size())throw EOFException("Invalid stream position");
    if(count>data.size()-offset){offset=data.size();throw EOFException("Truncated payload");}
    auto result=data.subspan(offset,count);offset+=count;return result;
}
}
