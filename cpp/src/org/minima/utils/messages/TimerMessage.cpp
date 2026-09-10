#include "TimerMessage.hpp"
#include <chrono>
namespace minima {
int64_t TimerMessage::currentTimeMillis(){return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
TimerMessage::TimerMessage(int64_t delay,Key type):TimerMessage(delay,std::move(type),currentTimeMillis()){}
TimerMessage::TimerMessage(int64_t delay,Key type,int64_t now):Message(std::move(type)),mTimer(std::bit_cast<int64_t>(uint64_t(now)+uint64_t(delay))),mDelay(delay){}
TimerMessage::TimerMessage(int64_t delay,const std::shared_ptr<Message>& message):TimerMessage(delay,message?message->getMessageType():Key()) {if(!message)throw std::invalid_argument("Null message");mContents=message->getAllContents();}
std::string TimerMessage::toString() const {return toStringAt(currentTimeMillis());}
std::string TimerMessage::toStringAt(int64_t now) const {return "Timer:"+std::to_string(mTimer)+" Delay:"+std::to_string(std::bit_cast<int64_t>(uint64_t(mTimer)-uint64_t(now)))+" milli Msg:"+Message::toString();}
}
