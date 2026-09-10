#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/MinimaUncaughtException.hpp"
#include <future>
#include <iostream>
#include <sstream>
using namespace minima;
namespace {
void require(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
struct Handler:NativeUncaughtExceptionHandler {
    std::function<void(const NativeThread*,const std::shared_ptr<MinimaLogger::Throwable>&)> call;
    std::function<void()> destroy;
    ~Handler(){if(destroy)destroy();}
    void uncaughtException(const NativeThread* t,const std::shared_ptr<MinimaLogger::Throwable>& e)override{call(t,e);}
};
struct TextError:MinimaLogger::Throwable {
    Kind type;explicit TextError(Kind value):type(value){}
    MinimaLogger::String toString()const override{return u"explicit";}
    std::optional<std::vector<MinimaLogger::String>> getStackTrace()const override{return std::vector<MinimaLogger::String>{};}
    Kind kind()const noexcept override{return type;}
};
}
int main()try{
    using Kind=MinimaLogger::Throwable::Kind;
    struct SubTimeout:JavaTimeoutException{using JavaTimeoutException::JavaTimeoutException;};
    struct SubStore:JavaMVStoreException{using JavaMVStoreException::JavaMVStoreException;};
    struct SubMemory:JavaOutOfMemoryError{using JavaOutOfMemoryError::JavaOutOfMemoryError;};
    SubTimeout timeout("timeout");SubStore store("store");SubMemory memory("memory");JavaError error("java.lang.LinkageError","linkage");std::bad_alloc allocation;
    JavaTypedException spoof("java.lang.OutOfMemoryError","merely named");
    require(nativeExceptionText(JavaOutOfMemoryError(""))=="java.lang.OutOfMemoryError: "&&nativeExceptionText(JavaTimeoutException(""))=="java.util.concurrent.TimeoutException: "&&nativeExceptionText(JavaMVStoreException(""))=="org.h2.mvstore.MVStoreException: ","Empty exception message became null");
    require(NativeThrowable(timeout).kind()==Kind::Timeout&&NativeThrowable(store).kind()==Kind::MVStore&&NativeThrowable(memory).kind()==Kind::OutOfMemory&&NativeThrowable(allocation).kind()==Kind::OutOfMemory&&NativeThrowable(error).kind()==Kind::Error&&NativeThrowable(spoof).kind()==Kind::Exception,"Throwable classification depends on names or misses subtypes");
    auto old=std::make_shared<Handler>(),replacement=std::make_shared<Handler>();std::atomic<int> calls{0};old->call=[](auto,auto){throw std::runtime_error("Old default used");};
    replacement->call=[&](auto thread,auto throwable){require(thread->id==std::this_thread::get_id()&&thread->name==u"waiting-worker","Lost actual thread identity");require(throwable->kind()==Kind::Timeout,"Lost native error type");++calls;};
    setDefaultUncaughtExceptionHandler(old);std::promise<void> entered,release;auto released=release.get_future();
    std::thread worker([&]{NativeThread thread{u"waiting-worker"};runNativeThread(thread,[&]{entered.set_value();released.wait();throw SubTimeout("timeout");});});entered.get_future().wait();setDefaultUncaughtExceptionHandler(replacement);release.set_value();worker.join();require(calls==1,"Default was captured at worker startup");
    // Both registries must release retired handlers outside their locks.
    int retired=0;old->destroy=[&]{++retired;setDefaultUncaughtExceptionHandler(nullptr);};setDefaultUncaughtExceptionHandler(old);old.reset();setDefaultUncaughtExceptionHandler(replacement);require(retired==1&&!getDefaultUncaughtExceptionHandler(),"Default retirement cannot reenter");
    auto captured=std::make_shared<Handler>();captured->destroy=[&]{++retired;MinimaUncaughtException nested;};setDefaultUncaughtExceptionHandler(captured);MinimaUncaughtException first;setDefaultUncaughtExceptionHandler(nullptr);captured.reset();MinimaUncaughtException second;require(retired==2,"Captured-handler retirement cannot reenter constructor");
    MinimaLogger::Runtime logging;logging.println=[](const auto&){};MinimaLogger::setRuntime(logging);NativeThread thread{u"test"};
    for(auto kind:{Kind::OutOfMemory,Kind::MVStore,Kind::Timeout}){int halted=0;MinimaUncaughtException h([&](int code){require(code==0,"Wrong halt code");++halted;});auto e=std::make_shared<TextError>(kind);h.uncaughtException(&thread,e);h.uncaughtException(nullptr,e);require(halted==1,"Halt return/duplicate behavior changed");}
    auto failed=std::make_shared<Handler>();failed->call=[&](auto,auto){++calls;throw std::runtime_error("expected");};setDefaultUncaughtExceptionHandler(failed);runNativeThread(thread,[]{throw std::runtime_error("worker");});require(calls==2,"Handler exception escaped thread boundary");
    // Do not manufacture a catch-site stack for a foreign C++ throw.
    failed->call=[&](auto,auto e){require(e->toString()==u"native.UnknownException: Non-standard C++ exception"&&e->getStackTrace()->empty(),"Invented foreign error stack");++calls;};runNativeThread(thread,[]{throw 7;});require(calls==3,"Foreign throw not dispatched");
    setDefaultUncaughtExceptionHandler(nullptr);std::ostringstream output;auto saved=std::cerr.rdbuf(output.rdbuf());runNativeThread(thread,[]{throw std::runtime_error("fallback");});std::cerr.rdbuf(saved);require(output.str()=="Exception in thread \"test\" java.lang.RuntimeException: fallback\n","Missing default stderr behavior");
    MinimaLogger::setRuntime({});std::cout<<"Native thread handler: actual identity, late lookup, reentrant retirement, typed/subtype classification, injected halt, foreign throw and stderr fallback PASS\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
