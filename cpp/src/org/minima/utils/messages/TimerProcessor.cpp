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
void TimerProcessor::createTimerProcessor(){auto p=createManual();auto& registry=timers();std::lock_guard lock(registry.lock);registry.instances.push_back(p);auto name=nextNativeThreadName();p->mMainThread=std::thread([p,name=std::move(name)]()mutable{NativeThread thread{std::move(name)};runNativeThread(thread,[&]{p->run();});});}
std::shared_ptr<TimerProcessor> TimerProcessor::getTimerProcessor(){auto& registry=timers();std::lock_guard lock(registry.lock);return registry.instances.empty()?nullptr:registry.instances.back();}
void TimerProcessor::stopTimerProcessor(){auto p=getTimerProcessor();if(!p)throw JavaNullPointerException("TimerProcessor is not initialized");p->stop();}
void TimerProcessor::stop(){{std::lock_guard lock(mSleepLock);mRunning=false;}mSleep.notify_all();}
void TimerProcessor::join(){std::lock_guard lock(mJoinLock);if(mMainThread.joinable()){if(mMainThread.get_id()==std::this_thread::get_id())mMainThread.detach();else mMainThread.join();}}
void TimerProcessor::PostMessage(std::shared_ptr<TimerMessage> message){std::lock_guard lock(mMessagesLock);if(message)mTimerMessages.push_back(std::move(message));else {MinimaLogger::log("NULL TIMER Message attempt:");NativeStackTrace trace;for(const auto& frame:trace.nativeStackTrace())MinimaLogger::log("     "+frame);}}
int32_t TimerProcessor::getSize() const {std::lock_guard lock(mMessagesLock);return int32_t(mTimerMessages.size());}
void TimerProcessor::printAllMessages() const {std::lock_guard lock(mMessagesLock);for(const auto& msg:mTimerMessages)MinimaLogger::log(msg->toString(),false);}
void TimerProcessor::pollAt(int64_t time){std::lock_guard lock(mMessagesLock);std::vector<std::shared_ptr<TimerMessage>> keep;for(const auto& msg:mTimerMessages){if(msg->getTimer()<time){auto processor=msg->getProcessor();if(!processor)throw JavaNullPointerException("null timer processor");if(processor->isRunning())processor->PostMessage(msg);else MinimaLogger::log("Timer Message NOT run as processor shutdown.. "+msg->toString());}else keep.push_back(msg);}mTimerMessages=std::move(keep);}
void TimerProcessor::run(){while(mRunning){pollAt(TimerMessage::currentTimeMillis());std::unique_lock lock(mSleepLock);mSleep.wait_for(lock,std::chrono::milliseconds(1000),[this]{return !mRunning;});}}
}
