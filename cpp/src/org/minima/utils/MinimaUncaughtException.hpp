#pragma once
#include "support/NativeThread.hpp"

namespace minima {
class MinimaUncaughtException:public NativeUncaughtExceptionHandler {
    std::mutex mLastLock;
    MinimaLogger::String mLastError=u"***";
    std::function<void(int)> mHalt;
public:
    // Empty halt callback binds Runtime.halt to immediate native _Exit. A
    // supplied callback is a platform/test boundary, not a graceful shutdown.
    explicit MinimaUncaughtException(std::function<void(int)> halt={});
    void uncaughtException(const NativeThread* thread,
        const std::shared_ptr<MinimaLogger::Throwable>& error) override;
};
}
