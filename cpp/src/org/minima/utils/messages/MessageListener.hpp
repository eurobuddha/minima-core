#pragma once
#include "Message.hpp"
namespace minima {
class MessageListener {
public:
    virtual ~MessageListener()=default;
    virtual void processMessage(std::shared_ptr<Message> message)=0;
};
}
