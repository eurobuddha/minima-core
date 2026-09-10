#include "MinimaUncaughtException.hpp"
#include "support/JavaExceptions.hpp"
#include <cstdlib>

namespace minima {
namespace {
std::mutex capturedLock;
std::shared_ptr<NativeUncaughtExceptionHandler> capturedDefault;
}
MinimaUncaughtException::MinimaUncaughtException(std::function<void(int)> halt):mHalt(std::move(halt)){
    if(!mHalt)mHalt=[](int status){std::_Exit(status);};
    auto replacement=getDefaultUncaughtExceptionHandler();
    {std::lock_guard lock(capturedLock);capturedDefault.swap(replacement);}
}
void MinimaUncaughtException::uncaughtException(const NativeThread* thread,
    const std::shared_ptr<MinimaLogger::Throwable>& error){
    if(!error)throw JavaNullPointerException("null throwable");
    auto checkerror=error->toString();
    {
        std::lock_guard lock(mLastLock);
        if(!mLastError)throw JavaNullPointerException("null last error");
        if(mLastError==checkerror)return;
    }
    if(!thread)throw JavaNullPointerException("null thread");
    MinimaLogger::log(MinimaLogger::String(u"[!] UNCAUGHT EXCEPTION at THREAD "+thread->name));
    MinimaLogger::logUncaught(error,true);
    {std::lock_guard lock(mLastLock);mLastError=checkerror;}
    using Kind=MinimaLogger::Throwable::Kind;
    if(error->kind()==Kind::OutOfMemory){
        MinimaLogger::log("[!] MEMORY ERROR.. SHUTTING DOWN");mHalt(0);return;
    }else if(error->kind()==Kind::MVStore){
        MinimaLogger::log("[!] H2 DATABASE MVStoreException.. SHUTTING DOWN");mHalt(0);return;
    }else if(error->kind()==Kind::Timeout){
        MinimaLogger::log("[!] Concurrent Timeout Exception.. SHUTTING DOWN");mHalt(0);return;
    }
    std::shared_ptr<NativeUncaughtExceptionHandler> handler;
    {std::lock_guard lock(capturedLock);handler=capturedDefault;}
    if(!handler)throw JavaNullPointerException("null default uncaught handler");
    handler->uncaughtException(thread,error);
}
}
