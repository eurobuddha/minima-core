#pragma once
#include "ByteIO.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
namespace minima {
class NIOSocket {
public:
    virtual ~NIOSocket()=default;
    // JDK channel conventions: read EOF=-1; would-block=0; write may be partial.
    virtual int32_t read(std::span<uint8_t> destination)=0;
    virtual int32_t write(std::span<const uint8_t> source)=0;
    virtual void close()=0;
};
// Owns an already-connected POSIX descriptor, as the sibling injectFd does.
class PosixNIOSocket final : public NIOSocket {
    int mDescriptor;
    mutable std::mutex mIOLock;
public:
    explicit PosixNIOSocket(int descriptor);
    ~PosixNIOSocket() override;
    PosixNIOSocket(const PosixNIOSocket&)=delete;
    PosixNIOSocket& operator=(const PosixNIOSocket&)=delete;
    int descriptor() const {std::lock_guard lock(mIOLock);return mDescriptor;}
    int32_t read(std::span<uint8_t> destination) override;
    int32_t write(std::span<const uint8_t> source) override;
    void close() override;
};
class NIOSelectionKey {
public:
    static constexpr int32_t OP_READ=1,OP_WRITE=4;
    virtual ~NIOSelectionKey()=default;
    virtual bool isValid() const=0;
    virtual void interestOps(int32_t flags)=0;
    virtual void wakeup()=0;
    virtual void cancel()=0;
};
// Actual interest state used by a native selector; wakeup is supplied by it.
class NativeNIOSelectionKey final : public NIOSelectionKey {
    std::atomic<bool> mValid{true};
    std::atomic<int32_t> mInterest{OP_READ};
    std::function<void()> mWakeup;
public:
    explicit NativeNIOSelectionKey(std::function<void()> wakeup):mWakeup(std::move(wakeup)){}
    bool isValid() const override {return mValid;}
    int32_t interestOps() const {if(!isValid())throw std::runtime_error("CancelledKeyException");return mInterest;}
    void interestOps(int32_t flags) override {if(!isValid())throw std::runtime_error("CancelledKeyException");if(flags&~(OP_READ|OP_WRITE))throw std::invalid_argument("Invalid interest operations");mInterest=flags;}
    void wakeup() override {mWakeup();}
    void cancel() override {mValid=false;}
};
}
