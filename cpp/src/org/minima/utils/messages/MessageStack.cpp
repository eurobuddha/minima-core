#include "MessageStack.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaExceptions.hpp"
#include <iostream>
namespace minima {
void MessageStack::PostMessage(std::shared_ptr<Message> message){{std::lock_guard lock(mLock);mMessages.push_back(std::move(message));++mModificationCount;}notifyLock();}
std::shared_ptr<Message> MessageStack::getNextMessage(){std::lock_guard lock(mLock);if(mMessages.empty())return nullptr;auto message=mMessages.front();mMessages.pop_front();++mModificationCount;return message;}
bool MessageStack::isNextMessage() const {std::lock_guard lock(mLock);return !mMessages.empty();}
int32_t MessageStack::getSize() const {std::lock_guard lock(mLock);return int32_t(mMessages.size());}
void MessageStack::clear(){std::lock_guard lock(mLock);mMessages.clear();++mModificationCount;}
void MessageStack::clearExcept(const std::vector<Message::Key>& exclude){std::lock_guard lock(mLock);std::deque<std::shared_ptr<Message>> keep;for(const auto& msg:mMessages){if(!msg)throw std::invalid_argument("Null message");for(const auto& type:exclude){if((!type&&!msg->getMessageType())||(type&&msg->getMessageType()&&unicode::decodeUTF8(*type)==unicode::decodeUTF8(*msg->getMessageType()))){keep.push_back(msg);break;}}}mModificationCount+=uint32_t(keep.size())+1;mMessages=std::move(keep);}
void MessageStack::clearExceptString(const std::string& exclude){auto pattern=unicode::decodeUTF8(exclude);std::lock_guard lock(mLock);std::deque<std::shared_ptr<Message>> keep;for(const auto& msg:mMessages){if(!msg)throw std::invalid_argument("Null message");if(unicode::decodeUTF8(msg->getMessageType().value()).find(pattern)!=std::u16string::npos)keep.push_back(msg);}mModificationCount+=uint32_t(keep.size())+1;mMessages=std::move(keep);}
void MessageStack::printAllMessages() const {
    std::lock_guard lock(mLock);size_t index=0;auto expected=mModificationCount;
    // LinkedList.ListItr checks size in hasNext, then modCount in next. Keep no
    // deque iterator/reference across logging: the callback may mutate this queue.
    while(index<mMessages.size()){
        if(expected!=mModificationCount)throw JavaTypedException("java.util.ConcurrentModificationException","");
        auto message=mMessages[index++];if(!message)throw JavaNullPointerException("Null message");
        MinimaLogger::log("MSG_PROC ["+std::to_string(index-1)+"] : "+message->toString(),false);
    }
}
}
