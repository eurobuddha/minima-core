#pragma once
#include "org/minima/utils/MinimaLogger.hpp"
#include <thread>
#include <condition_variable>

namespace minima {
struct NativeThread {
    struct InterruptState {
        std::mutex lock;
        std::condition_variable wake;
        bool pending=false;
    };
    std::u16string name;
    std::thread::id id=std::this_thread::get_id();
    std::shared_ptr<InterruptState> interruption=std::make_shared<InterruptState>();
    void interrupt() const;
    bool isInterrupted() const;
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
// Java Thread.sleep semantics: a pending interrupt is consumed and throws,
// including for zero delay. Negative delay is rejected before consuming it.
void nativeThreadSleep(int64_t milliseconds);
void interruptCurrentNativeThread();
bool nativeThreadInterrupted();
}
