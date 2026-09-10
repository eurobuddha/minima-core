#include "MessageProcessor.hpp"
#include "TimerProcessor.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
namespace minima {
std::mutex MessageProcessor::sTraceLock;
bool MessageProcessor::sTrace=false;
std::string MessageProcessor::sTraceFilter,MessageProcessor::sLanguage;
MessageProcessor::MessageProcessor(std::string name,std::function<void()> releaseLocks):mName(std::move(name)),mReleaseLocks(std::move(releaseLocks)){if(!mReleaseLocks)throw std::invalid_argument("MessageProcessor requires database lock cleanup");}
MessageProcessor::~MessageProcessor(){stopMessageProcessor();joinMessageProcessor();}
void MessageProcessor::startMessageProcessor(){std::lock_guard lock(mJoinLock);if(mMainThread.joinable()||mShutDownComplete||mExecuting)throw std::logic_error("Processor already started");auto self=shared_from_this();auto name=unicode::decodeUTF8(mName);mMainThread=std::thread([self,name=std::move(name)]()mutable{NativeThread thread{std::move(name)};runNativeThread(thread,[&]{self->run();});});}
void MessageProcessor::joinMessageProcessor(){std::lock_guard lock(mJoinLock);if(mMainThread.joinable()){if(mMainThread.get_id()==std::this_thread::get_id())mMainThread.detach();else mMainThread.join();}}
void MessageProcessor::setTrace(bool trace,std::string filter){std::lock_guard lock(sTraceLock);sTrace=trace;sTraceFilter=std::move(filter);}
void MessageProcessor::setTraceLanguage(std::string language){std::lock_guard lock(sTraceLock);sLanguage=std::move(language);}
bool MessageProcessor::isTrace() const {std::lock_guard lock(sTraceLock);return sTrace;}
std::string MessageProcessor::getTraceFilter() const {std::lock_guard lock(sTraceLock);return sTraceFilter;}
std::shared_ptr<Message> MessageProcessor::getLastMessage() const {std::lock_guard lock(mLock);return mLastMessage;}
bool MessageProcessor::checkTraceFilter(const std::string& text) const {std::lock_guard lock(sTraceLock);return sTrace&&unicode::decodeUTF8(javaLowerCase(text,sLanguage)).find(unicode::decodeUTF8(javaLowerCase(sTraceFilter,sLanguage)))!=std::u16string::npos;}
void MessageProcessor::stopMessageProcessor(){{std::lock_guard lock(mLock);mRunning=false;}notifyLock();}
void MessageProcessor::waitToShutDown(int64_t timeout){int64_t waited=0;while(!isShutdownComplete()){std::this_thread::sleep_for(std::chrono::milliseconds(250));waited=std::bit_cast<int64_t>(uint64_t(waited)+250);if(waited>timeout){MinimaLogger::log("Failed to shutdown in "+std::to_string(timeout)+" millisecs for "+mName);mShutDownComplete=true;stopMessageProcessor();return;}}}
void MessageProcessor::PostTimerMessage(const std::shared_ptr<TimerMessage>& message){if(!message)throw JavaNullPointerException("null timer message");message->setProcessor(shared_from_this());auto timers=TimerProcessor::getTimerProcessor();if(!timers)throw JavaNullPointerException("TimerProcessor is not initialized");timers->PostMessage(message);}
void MessageProcessor::run(){
    if(mExecuting.exchange(true))throw std::logic_error("Processor already executing");
    if(isTrace())MinimaLogger::log("["+mName+"] (stack:"+std::to_string(getSize())+") START",false);
    while(isRunning()){
        auto msg=getNextMessage();
        while(msg&&isRunning()){
            // Java finally runs even if a catch block's logging throws; a
            // cleanup failure replaces any exception already in flight.
            try {
                try {
                    auto trace=msg->toString();int64_t begin=checkTraceFilter(trace)?TimerMessage::currentTimeMillis():0;
                    {std::lock_guard lock(mLock);mLastMessage=msg;}
                    processMessage(msg);
                    if(checkTraceFilter(trace)){
                        auto elapsed=std::bit_cast<int64_t>(uint64_t(TimerMessage::currentTimeMillis())-uint64_t(begin));
                        MinimaLogger::log("TRACE > ["+mName+"] (stack:"+std::to_string(getSize())+") process_time_milli:"+std::to_string(elapsed)+" \t"+msg->toString(),false);
                    }
                }catch(const std::exception& error){
                    auto throwable=std::make_shared<NativeThrowable>(error);
                    auto kind=throwable->kind();
                    if(kind==MinimaLogger::Throwable::Kind::Error||kind==MinimaLogger::Throwable::Kind::OutOfMemory){
                        MinimaLogger::log("**SERIOUS ERROR "+msg->getMessageType().value_or("null")+" "+unicode::encodeUTF8(throwable->toString().value_or(u"null")));
                        auto frames=throwable->getStackTrace();
                        for(const auto& frame:*frames)MinimaLogger::log(MinimaLogger::String(u"     "+frame.value()));
                    }else{
                        MinimaLogger::log("MESSAGE PROCESSING ERROR @ "+msg->getMessageType().value_or("null"));
                        MinimaLogger::log(throwable);
                    }
                }
            }catch(...){mReleaseLocks();throw;}
            mReleaseLocks();
            msg=getNextMessage();
        }
        std::unique_lock lock(mLock);mNotify.wait(lock,[this]{return !mMessages.empty()||!mRunning;});
    }
    if(isTrace())MinimaLogger::log("["+mName+"] (stack:"+std::to_string(getSize())+") SHUTDOWN",false);
    mRunning=false;mShutDownComplete=true;
}
}
