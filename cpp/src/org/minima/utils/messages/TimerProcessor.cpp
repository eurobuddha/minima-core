#include "TimerProcessor.hpp"
#include "MessageProcessor.hpp"
#include <iostream>
#include "support/NativeThread.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
namespace minima {
namespace {
struct Timers {
    std::mutex lock;
    std::vector<std::shared_ptr<TimerProcessor>> instances;
    ~Timers(){for(const auto& p:instances)p->stop();for(const auto& p:instances)p->join();}
};
Timers& timers(){static Timers value;return value;}
}
TimerProcessor::~TimerProcessor(){stop();join();}
void TimerProcessor::createTimerProcessor(){auto p=createManual();auto& registry=timers();std::lock_guard lock(registry.lock);registry.instances.push_back(p);p->mWorkerThread=std::make_shared<NativeThread>(NativeThread{nextNativeThreadName()});p->mMainThread=std::thread([p]{p->mWorkerThread->id=std::this_thread::get_id();runNativeThread(*p->mWorkerThread,[&]{p->run();});});}
std::shared_ptr<TimerProcessor> TimerProcessor::getTimerProcessor(){auto& registry=timers();std::lock_guard lock(registry.lock);return registry.instances.empty()?nullptr:registry.instances.back();}
void TimerProcessor::stopTimerProcessor(){auto p=getTimerProcessor();if(!p)throw JavaNullPointerException("TimerProcessor is not initialized");p->stop();}
void TimerProcessor::stop(){mRunning=false;try{if(mWorkerThread)mWorkerThread->interrupt();}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){};}
void TimerProcessor::join(){std::lock_guard lock(mJoinLock);if(mMainThread.joinable()){if(mMainThread.get_id()==std::this_thread::get_id())mMainThread.detach();else mMainThread.join();}}
void TimerProcessor::PostMessage(std::shared_ptr<TimerMessage> message){std::lock_guard lock(mMessagesLock);if(message)mTimerMessages->add(std::move(message));else {MinimaLogger::log("NULL TIMER Message attempt:");NativeStackTrace trace;for(const auto& frame:trace.nativeStackTrace())MinimaLogger::log("     "+frame);}}
int32_t TimerProcessor::getSize() const {std::lock_guard lock(mMessagesLock);return int32_t(mTimerMessages->size());}
void TimerProcessor::printAllMessages() const {
    std::lock_guard lock(mMessagesLock);
    // Retain the iterated ArrayList even if a reentrant callback replaces it.
    auto messages=mTimerMessages;
    for(auto msg:messages->range()){
        if(!msg)throw JavaNullPointerException("null timer message");
        MinimaLogger::log(msg->toString(),false);
    }
}
void TimerProcessor::pollAt(int64_t time){
    std::lock_guard lock(mMessagesLock);
    auto messages=mTimerMessages;
    auto keep=std::make_shared<TimerList>();
    for(auto msg:messages->range()){
        if(!msg){MinimaLogger::log("Timer Message is NULL.. ?");continue;}
        if(msg->getTimer()<time){
            auto processor=msg->getProcessor();
            if(!processor)throw JavaNullPointerException("null timer processor");
            if(processor->isRunning())processor->PostMessage(msg);
            else MinimaLogger::log("Timer Message NOT run as processor shutdown.. "+msg->toString());
        }else keep->add(msg);
    }
    mTimerMessages=std::move(keep);
}
void TimerProcessor::run(){
    while(mRunning){
        // Java reads wall time after acquiring the list monitor.
        {std::lock_guard lock(mMessagesLock);pollAt(TimerMessage::currentTimeMillis());}
        try{nativeThreadSleep(1000);}catch(const JavaTypedException& e){if(e.className()!="java.lang.InterruptedException")throw;mRunning=false;}
    }
}
}
