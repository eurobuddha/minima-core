#include "org/minima/system/network/minima/NIOServer.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeShutdownHooks.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <iostream>
#include <cstdlib>
using namespace minima;using namespace minima::json;
namespace {
std::mutex outputLock;
void emit(const std::string& event,const std::string& value){JSONObject row;row.put("event",event);row.put("value",value);std::lock_guard lock(outputLock);std::cout<<row.toJSONString()<<std::endl;}
class Handler:public NativeUncaughtExceptionHandler {
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& error)override{
        auto text=unicode::encodeUTF8(error->toString().value());emit("uncaught",unicode::encodeUTF8(thread->name)+":"+text.substr(0,text.find(':')));
    }
};
}
int main(int argc,char** argv){
    int interruptAt=argc>1?std::stoi(argv[1]):0;int stage=0;
    MinimaLogger::Runtime runtime;runtime.println=[&](const auto& line){auto text=unicode::encodeUTF8(line);text=text.substr(text.find(" : ")+3);emit("log",text);if(text=="Started.. waiting.."||text=="Started again.. waiting.."||text=="Stopping.."){if(++stage==interruptAt)interruptCurrentNativeThread();}};MinimaLogger::setRuntime(runtime);
    setDefaultUncaughtExceptionHandler(std::make_shared<Handler>());addNativeShutdownHook([]{emit("hook","exit");});
    try{NIOServer::main(nullptr);emit("returned","unexpected");}
    catch(const std::exception& e){auto text=javaExceptionText(e);emit("caught",text.substr(0,text.find(':'))+":"+(nativeThreadInterrupted()?"true":"false"));char value;std::cin.get(value);std::_Exit(7);}
    return 2;
}
