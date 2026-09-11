#pragma once
#include "JavaExceptions.hpp"
#include "ByteIO.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <functional>
#include <optional>
namespace minima {
// External method boundary only. This is NOT an implementation of jnifunctions
// or its missing native library. Tests explicitly install a fixture provider.
struct JNICommandRuntime {
    using Bytes=wire::Bytes;
    using Buffer=std::span<uint8_t>;
    inline static thread_local std::function<void()> sayHello=[] {
        throw JavaError("java.lang.UnsatisfiedLinkError","'void org.minima.utils.jni.jnifunctions.sayHello()'");
    };
    inline static thread_local std::function<std::optional<Bytes>(Buffer,int32_t,Buffer,Buffer)> hashHeaderWithDiff=[](Buffer,int32_t,Buffer,Buffer)->std::optional<Bytes> {
        throw JavaError("java.lang.UnsatisfiedLinkError","'byte[] org.minima.utils.jni.jnifunctions.hashHeaderWithDiff(byte[], int, byte[], byte[])'");
    };
    static void copyNonce(const Bytes& nonce,Bytes& data) {
        if(data.size()<4||nonce.size()>data.size()-4)throw JavaArrayIndexOutOfBoundsException("nonce copy exceeds header");
        std::copy(nonce.begin(),nonce.end(),data.begin()+4);
    }
};
}
