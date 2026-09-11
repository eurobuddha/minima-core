#include "support/NativeShutdownHooks.hpp"
#include "support/NativeThread.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <latch>
#include <cstdlib>
#include <iostream>
using namespace minima;
namespace {
std::mutex printLock;
void print(const std::string& value){std::lock_guard guard(printLock);std::cout<<value<<std::endl;}
class Handler final:public NativeUncaughtExceptionHandler {
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& error) override{
        print("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(error->toString().value()));
    }
};
}
int main(int argc,char** argv){
    if(argc!=3)return 2;std::string mode=argv[1];int count=std::stoi(argv[2]);
    auto started=std::make_shared<std::latch>(count);setDefaultUncaughtExceptionHandler(std::make_shared<Handler>());
    for(int index=0;index<count;++index)addNativeShutdownHook([index,mode,started]{
        started->count_down();started->wait();print("hook:"+std::to_string(index)+":"+currentNativeThreadName());
        if(mode=="throw"&&index==0)throw JavaTypedException("java.lang.IllegalStateException","fixture hook");
        if(mode=="late"&&index==0){try{addNativeShutdownHook([]{});}catch(const JavaTypedException& error){print("late:"+error.className());}}
    });
    print("READY");
    if(mode=="signal"){std::this_thread::sleep_for(std::chrono::seconds(60));return 2;}
    if(mode=="halt")std::_Exit(7);
    if(mode=="exit")std::exit(9);
    return 0;
}
