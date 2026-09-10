#include "NativeThread.hpp"
#include "NativeThrowable.hpp"
#include <atomic>
#include <cstdio>
#include <iostream>

namespace minima {
namespace {
std::mutex handlerLock;
std::shared_ptr<NativeUncaughtExceptionHandler> defaultHandler;
std::atomic<uint64_t> threadNumber{0};
thread_local const NativeThread* currentThread=nullptr;
struct UnknownThrowable:MinimaLogger::Throwable {
    MinimaLogger::String toString() const override{return u"native.UnknownException: Non-standard C++ exception";}
    std::optional<std::vector<MinimaLogger::String>> getStackTrace() const override{return std::vector<MinimaLogger::String>{};}
};
void dispatch(const NativeThread& thread,const std::shared_ptr<MinimaLogger::Throwable>& error){
    if(auto handler=getDefaultUncaughtExceptionHandler())handler->uncaughtException(&thread,error);
    else {
        std::cerr<<"Exception in thread \""<<unicode::encodeUTF8(thread.name)<<"\" "
                 <<unicode::encodeUTF8(error->toString().value_or(u"null"))<<'\n';
        if(auto frames=error->getStackTrace())for(const auto& frame:*frames)
            std::cerr<<"\tat "<<unicode::encodeUTF8(frame.value_or(u"null"))<<'\n';
        std::cerr.flush();
    }
}
}
std::shared_ptr<NativeUncaughtExceptionHandler> getDefaultUncaughtExceptionHandler(){
    std::lock_guard lock(handlerLock);return defaultHandler;
}
void setDefaultUncaughtExceptionHandler(std::shared_ptr<NativeUncaughtExceptionHandler> handler){
    {std::lock_guard lock(handlerLock);defaultHandler.swap(handler);}
    // Retiring handlers may reenter the registry. Release outside its lock.
}
std::u16string nextNativeThreadName(){return unicode::decodeUTF8("Thread-"+std::to_string(threadNumber.fetch_add(1)));}
std::string currentNativeThreadName(){return currentThread?unicode::encodeUTF8(currentThread->name):"main";}
void runNativeThread(const NativeThread& thread,const std::function<void()>& run) noexcept {
    struct Restore{const NativeThread* previous;~Restore(){currentThread=previous;}}restore{currentThread};currentThread=&thread;
    try {
        try {run();}
        catch(const std::exception& error){dispatch(thread,std::make_shared<NativeThrowable>(error));}
        catch(...){
            // A non-standard C++ exception has no recorded Java/native frames.
            dispatch(thread,std::make_shared<UnknownThrowable>());
        }
    }catch(...){
        // This includes a handler failure and failure while materializing its
        // native throwable. Do not terminate unrelated threads via std::terminate.
        std::fputs("Exception while dispatching native uncaught exception\n",stderr);
    }
}
}
