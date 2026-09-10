#include "MessageStack.hpp"
#include <iostream>
namespace minima {
void MessageStack::PostMessage(std::shared_ptr<Message> message){{std::lock_guard lock(mLock);mMessages.push_back(std::move(message));}notifyLock();}
std::shared_ptr<Message> MessageStack::getNextMessage(){std::lock_guard lock(mLock);if(mMessages.empty())return nullptr;auto message=mMessages.front();mMessages.pop_front();return message;}
bool MessageStack::isNextMessage() const {std::lock_guard lock(mLock);return !mMessages.empty();}
int32_t MessageStack::getSize() const {std::lock_guard lock(mLock);return int32_t(mMessages.size());}
void MessageStack::clear(){std::lock_guard lock(mLock);mMessages.clear();}
void MessageStack::clearExcept(const std::vector<Message::Key>& exclude){std::lock_guard lock(mLock);std::deque<std::shared_ptr<Message>> keep;for(const auto& msg:mMessages){if(!msg)throw std::invalid_argument("Null message");for(const auto& type:exclude){if((!type&&!msg->getMessageType())||(type&&msg->getMessageType()&&unicode::decodeUTF8(*type)==unicode::decodeUTF8(*msg->getMessageType()))){keep.push_back(msg);break;}}}mMessages=std::move(keep);}
void MessageStack::clearExceptString(const std::string& exclude){auto pattern=unicode::decodeUTF8(exclude);std::lock_guard lock(mLock);std::deque<std::shared_ptr<Message>> keep;for(const auto& msg:mMessages){if(!msg)throw std::invalid_argument("Null message");if(unicode::decodeUTF8(msg->getMessageType().value()).find(pattern)!=std::u16string::npos)keep.push_back(msg);}mMessages=std::move(keep);}
void MessageStack::printAllMessages() const {std::lock_guard lock(mLock);size_t count=0;for(const auto& message:mMessages){if(!message)throw std::invalid_argument("Null message");std::clog<<"MSG_PROC ["<<count++<<"] : "<<message->toString()<<'\n';}}
}
