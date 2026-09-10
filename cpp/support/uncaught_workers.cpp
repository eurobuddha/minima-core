#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include <latch>
using namespace minima;using namespace minima::json;
namespace {
std::latch started(1);
std::string mode;JSONArray events;void record(std::string text){events.add(std::move(text));}
class Worker:public MessageProcessor{
    Worker():MessageProcessor("worker",[this]{record("cleanup");if(mode=="cleanup"||mode=="logandcleanup")throw JavaTypedException("java.lang.IllegalStateException","cleanup");if(mode!="catchlog")stopMessageProcessor();}){}
    void processMessage(std::shared_ptr<Message>)override{
        record("process");if(mode=="exception"||mode=="catchlog"||mode=="logandcleanup"||mode=="handlerthrows")throw JavaTypedException("java.lang.RuntimeException","handler");if(mode=="store")throw JavaMVStoreException("handler");if(mode=="timeout")throw JavaTimeoutException("handler");if(mode=="error")throw JavaError("java.lang.LinkageError","handler");if(mode=="oom")throw JavaOutOfMemoryError("handler");
    }
public:static auto create(){auto p=std::shared_ptr<Worker>(new Worker);p->startMessageProcessor();return p;}
};
struct Handler:NativeUncaughtExceptionHandler{
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& error)override{record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(error->toString().value_or(u"null")));if(mode=="handlerthrows")throw std::runtime_error("default");}
};
}
int main(int argc,char** argv)try{
    if(argc!=2)return 2;mode=argv[1];MinimaLogger::Runtime r;r.println=[](const auto& full){auto value=unicode::encodeUTF8(full);value=value.substr(value.find(" : ")+3);if(value.starts_with("     "))return;record("log:"+value);if(value.ends_with("START"))started.count_down();
        if(((mode=="catchlog"||mode=="logandcleanup"||mode=="handlerthrows")&&value.starts_with("MESSAGE PROCESSING"))||(mode=="startlog"&&value.ends_with("START"))||(mode=="endlog"&&value.ends_with("SHUTDOWN")))throw JavaTypedException("java.lang.IllegalArgumentException","logger");};MinimaLogger::setRuntime(r);setDefaultUncaughtExceptionHandler(std::make_shared<Handler>());
    if(mode=="timer"){
        TimerProcessor::createTimerProcessor();auto timer=TimerProcessor::getTimerProcessor();timer->PostMessage(std::make_shared<TimerMessage>(-100,"ORPHAN"));timer->join();record("pending:"+std::to_string(timer->getSize()));
        // A failed timer's run() leaves mRunning unchanged. Verify by a second
        // direct run reaching the same orphan, rather than exposing new API.
        bool stillRunning=false;try{timer->run();}catch(const JavaNullPointerException&){stillRunning=true;}record(std::string("running:")+(stillRunning?"true":"false"));timer->stop();
    }else{
        MessageProcessor::setTrace(mode=="startlog"||mode=="endlog","");auto worker=Worker::create();if(mode=="endlog")started.wait();if(mode!="startlog")worker->PostMessage("TEST");worker->joinMessageProcessor();record(std::string("running:")+(worker->isRunning()?"true":"false"));record(std::string("shutdown:")+(worker->isShutdownComplete()?"true":"false"));record("last:"+(worker->getLastMessage()?worker->getLastMessage()->getMessageType().value_or("null"):"null"));worker->stopMessageProcessor();
    }
    setDefaultUncaughtExceptionHandler(nullptr);MinimaLogger::setRuntime({});JSONObject out;out.put("events",events);std::cout<<out.toJSONString()<<'\n';
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
