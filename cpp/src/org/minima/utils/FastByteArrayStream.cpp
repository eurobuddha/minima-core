#include "FastByteArrayStream.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
namespace minima {
FastByteArrayStream::FastByteArrayStream(int32_t totalSize) {
    if(totalSize<0)throw std::invalid_argument("Negative initial size: "+std::to_string(totalSize));
    buf.resize(size_t(std::min(totalSize,SIZE_INCREASE)));
}
void FastByteArrayStream::writeData(const std::vector<uint8_t>* data,int32_t offset,int32_t length) {
    if(offset<0)return;
    const auto sourceSize=data?data->size():0;
    auto required=int32_t(uint32_t(count)+uint32_t(length));
    if(required>int32_t(buf.size())){
        auto capacity=int32_t(uint32_t(buf.size())+uint32_t(std::max(SIZE_INCREASE,length)));
        if(capacity<0)throw std::length_error("Negative array size");
        try{buf.resize(size_t(capacity));}catch(const std::bad_alloc&){std::clog<<"[!] SERIOUS OUT OF MEMORY ERROR at FastByteArrayStream\n";throw std::invalid_argument("OUT of Memory on FastByteArrayStream");}
    }
    if(!data)throw std::invalid_argument("Null byte array");
    if(length<0||size_t(offset)>sourceSize||size_t(length)>sourceSize-size_t(offset)||count<0||size_t(count)>buf.size()||size_t(length)>buf.size()-size_t(count))throw std::out_of_range("Array copy bounds");
    if(length)std::memmove(buf.data()+count,data->data()+offset,size_t(length));count=required;
}
void FastByteArrayStream::ensureCapacity(int64_t required) {
    if(required<=int64_t(buf.size()))return;
    if(required>INT32_MAX)throw std::bad_alloc();
    auto preferred=std::max(required,int64_t(buf.size())*2);
    constexpr int64_t softMax=INT32_MAX-8;
    buf.resize(size_t(preferred<=softMax?preferred:std::max(required,softMax)));
}
void FastByteArrayStream::write(int32_t value) {
    std::lock_guard lock(mLock);ensureCapacity(int64_t(count)+1);buf[size_t(count++)]=uint8_t(value);
}
void FastByteArrayStream::write(const std::vector<uint8_t>* data,int32_t offset,int32_t length) {
    std::lock_guard lock(mLock);
    if(!data)throw std::invalid_argument("Null byte array");
    if(offset<0||length<0||size_t(offset)>data->size()||size_t(length)>data->size()-size_t(offset))throw std::out_of_range("Array write bounds");
    ensureCapacity(int64_t(count)+length);if(length)std::memmove(buf.data()+count,data->data()+offset,size_t(length));count+=length;
}
void FastByteArrayStream::writeBytes(const std::vector<uint8_t>& data) {
    if(data.size()>INT32_MAX)throw std::length_error("Byte array too large");write(data,0,int32_t(data.size()));
}
void FastByteArrayStream::writeTo(std::ostream& output) const {
    std::lock_guard lock(mLock);output.write(reinterpret_cast<const char*>(buf.data()),count);if(!output)throw wire::IOException("Output stream write failed");
}
void FastByteArrayStream::reset(){std::lock_guard lock(mLock);count=0;}
std::vector<uint8_t> FastByteArrayStream::toByteArray() const {std::lock_guard lock(mLock);return {buf.begin(),buf.begin()+count};}
int32_t FastByteArrayStream::size() const {std::lock_guard lock(mLock);return count;}
std::string FastByteArrayStream::toString() const {std::lock_guard lock(mLock);return MiniString(std::span(buf).first(size_t(count))).toString();}
std::u16string FastByteArrayStream::toString(int32_t highByte) const {
    std::lock_guard lock(mLock);std::u16string result;result.reserve(size_t(count));for(int32_t i=0;i<count;++i)result+=char16_t(((uint32_t(highByte)&255)<<8)|buf[size_t(i)]);return result;
}
}
