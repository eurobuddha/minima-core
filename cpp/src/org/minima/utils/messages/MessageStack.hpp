#pragma once
#include "Message.hpp"
#include <condition_variable>
#include <deque>
namespace minima {
class MessageStack {
protected:
    mutable std::recursive_mutex mLock;
    std::condition_variable_any mNotify;
    std::deque<std::shared_ptr<Message>> mMessages;
    uint32_t mModificationCount=0;
    void notifyLock(){mNotify.notify_all();}
    std::shared_ptr<Message> getNextMessage();
public:
    virtual ~MessageStack()=default;
    void PostMessage(const std::string& type){PostMessage(std::make_shared<Message>(type));}
    void PostMessage(std::shared_ptr<Message> message);
    bool isNextMessage() const;
    int32_t getSize() const;
    void clear();
    void clearExcept(const std::vector<Message::Key>& exclude);
    void clearExceptString(const std::string& exclude);
    void printAllMessages() const;
};
}
