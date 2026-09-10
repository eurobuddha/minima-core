#pragma once
#include "support/ByteIO.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include <mutex>
#include <ostream>
namespace minima {
class FastByteArrayStream {
    static constexpr int32_t SIZE_INCREASE=256*1024;
    mutable std::recursive_mutex mLock;
    void ensureCapacity(int64_t required);
protected:
    std::vector<uint8_t> buf;
    int32_t count=0;
public:
    explicit FastByteArrayStream(int32_t totalSize);
    virtual ~FastByteArrayStream()=default;
    // This method is unsynchronized in Java; the NIO reader owns its instance.
    void writeData(const std::vector<uint8_t>* data,int32_t offset,int32_t length);
    void writeData(const std::vector<uint8_t>& data,int32_t offset,int32_t length){writeData(&data,offset,length);}
    void write(int32_t value);
    void write(const std::vector<uint8_t>* data,int32_t offset,int32_t length);
    void write(const std::vector<uint8_t>& data,int32_t offset,int32_t length){write(&data,offset,length);}
    void writeBytes(const std::vector<uint8_t>& data);
    void writeTo(std::ostream& output) const;
    void reset();
    std::vector<uint8_t> toByteArray() const;
    int32_t size() const;
    std::string toString() const;
    std::u16string toString(int32_t highByte) const;
    void close() {}
    void flush() {}
};
}
