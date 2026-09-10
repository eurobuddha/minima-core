#pragma once
#include <cstdlib>
#include <execinfo.h>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace minima {
// Capture when an exception is constructed, not when it is caught. Copies and
// rethrows retain these addresses. Symbol names are native, not JVM frame names.
class NativeStackTrace {
    std::vector<void*> mFrames;
    [[gnu::noinline]] static std::vector<void*> capture() {
        std::vector<void*> frames(64);
        for (;;) {
            int count=::backtrace(frames.data(),static_cast<int>(frames.size()));
            if(count<static_cast<int>(frames.size())) {frames.resize(count);return frames;}
            if(frames.size()>static_cast<size_t>(std::numeric_limits<int>::max()/2))return frames;
            frames.resize(frames.size()*2);
        }
    }
public:
    NativeStackTrace():mFrames(capture()){}
    virtual ~NativeStackTrace()=default;
    const std::vector<void*>& nativeFrameAddresses() const noexcept{return mFrames;}
    std::vector<std::string> nativeStackTrace() const {
        std::vector<std::string> result;
        if(mFrames.empty())return result;
        std::unique_ptr<char*,decltype(&std::free)> symbols(
            ::backtrace_symbols(mFrames.data(),static_cast<int>(mFrames.size())),&std::free);
        if(!symbols)throw std::bad_alloc();
        result.reserve(mFrames.size());
        for(size_t i=0;i<mFrames.size();++i)result.emplace_back(symbols.get()[i]);
        return result;
    }
};
}
