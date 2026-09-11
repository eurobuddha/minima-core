#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {struct TimerProcessorFixture {
    static auto list(const TimerProcessor& timer){std::lock_guard lock(timer.mMessagesLock);return timer.mTimerMessages;}
    static bool running(const TimerProcessor& timer){return timer.mRunning.load();}
};}
class Target:public MessageProcessor {
public:Target():MessageProcessor("timer-test",[]{}){stopMessageProcessor();}
protected:void processMessage(std::shared_ptr<Message>)override{}
};
class Probe:public TimerMessage {
    std::function<void()> callback;
public:Probe(std::string name,int64_t deadline,std::function<void()> action):TimerMessage(deadline,name,0),callback(std::move(action)){}
    std::string toString()const override{if(callback)callback();return getMessageType().value();}
};
static std::string classify(const std::exception& e){auto value=javaExceptionText(e);return value.substr(0,value.find(':'));}
int main(int argc,char** argv)try{
    if(argc!=4)return 2;std::string mode=argv[1],action=argv[2];int count=std::stoi(argv[3]);
    auto timer=TimerProcessor::createManual();auto target=std::make_shared<Target>();JSONArray events,logs;bool fired=false;
    auto added=std::make_shared<Probe>("C",INT64_MAX,[]{});added->setProcessor(target);
    std::function<void()> callback=[&]{
        if(fired)return;fired=true;events.add("size:"+std::to_string(timer->getSize()));
        if(action=="append")timer->PostMessage(added);
        else if(action=="throw")throw JavaTypedException("java.lang.IllegalStateException","callback");
        else if(action=="print")timer->printAllMessages();
        else if(action=="run"){
            interruptCurrentNativeThread();timer->run();events.add("nested:"+std::to_string(timer->getSize()));
            // The outer run still reaches its source sleep after the nested one.
            if(mode=="run")interruptCurrentNativeThread();
        }else if(action=="nullpost")timer->PostMessage(nullptr);
        events.add("after:"+std::to_string(timer->getSize()));
    };
    MinimaLogger::Runtime logging;logging.println=[&](const auto& line){
        auto text=unicode::encodeUTF8(line);auto begin=text.find(" : ");text=text.substr(begin+3);
        if(text.starts_with("     "))return;logs.add(text);
        if(action=="log_append"&&!fired){fired=true;events.add("size:"+std::to_string(timer->getSize()));timer->PostMessage(added);events.add("after:"+std::to_string(timer->getSize()));}
    };MinimaLogger::setRuntime(logging);
    for(int i=0;i<count;++i){auto probe=std::make_shared<Probe>(i==0?"A":"B",i==0?0:INT64_MAX,action=="log_append"?std::function<void()>{}:callback);probe->setProcessor(target);timer->PostMessage(probe);}
    if(action=="nullentry")TimerProcessorFixture::list(*timer)->add(nullptr);
    auto held=TimerProcessorFixture::list(*timer);std::string error="none";
    if(mode=="run")interruptCurrentNativeThread();
    try{if(mode=="run")timer->run();else timer->printAllMessages();}catch(const std::exception& e){error=classify(e);}
    JSONObject result;result.put("error",error);result.put("pending",nativeThreadInterrupted());result.put("running",TimerProcessorFixture::running(*timer));result.put("events",events);result.put("logs",logs);
    auto names=[](const auto& list){JSONArray values;for(auto message:list->snapshot())values.add(message?JSONValue(message->getMessageType().value()):JSONValue());return values;};
    result.put("remaining",names(TimerProcessorFixture::list(*timer)));result.put("original",names(held));result.put("sameList",held==TimerProcessorFixture::list(*timer));
    MinimaLogger::setRuntime({});timer->stop();std::cout<<result.toJSONString()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
