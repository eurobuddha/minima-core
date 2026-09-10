#pragma once
#include "org/minima/utils/MinimaLogger.hpp"
#include <thread>

namespace minima {
struct NativeThread {
    std::u16string name;
    std::thread::id id=std::this_thread::get_id();
};
struct NativeUncaughtExceptionHandler {
    virtual ~NativeUncaughtExceptionHandler()=default;
    virtual void uncaughtException(const NativeThread* thread,
        const std::shared_ptr<MinimaLogger::Throwable>& error)=0;
};
// Default is resolved when the exception escapes run(), not when the worker
// starts. Like ThreadGroup, no handler means printing to stderr. An exception
// from the handler is ignored by the outer thread boundary, not by direct calls.
std::shared_ptr<NativeUncaughtExceptionHandler> getDefaultUncaughtExceptionHandler();
void setDefaultUncaughtExceptionHandler(std::shared_ptr<NativeUncaughtExceptionHandler> handler);
void runNativeThread(const NativeThread& thread,const std::function<void()>& run) noexcept;
std::u16string nextNativeThreadName();
std::string currentNativeThreadName();
}
