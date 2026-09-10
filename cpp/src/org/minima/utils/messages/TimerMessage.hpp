#pragma once
#include "Message.hpp"
namespace minima {
class MessageProcessor;
class TimerMessage : public Message {
    friend class NIOManagerFixture;
    int64_t mTimer;
    [[maybe_unused]] int64_t mDelay;
    std::weak_ptr<MessageProcessor> mProcessor;
public:
    TimerMessage(int64_t delay,Key type);
    TimerMessage(int64_t delay,const std::shared_ptr<Message>& message);
    // Explicit clock is a native test adapter, preserving signed Java overflow.
    TimerMessage(int64_t delay,Key type,int64_t now);
    void setProcessor(const std::shared_ptr<MessageProcessor>& processor){mProcessor=processor;}
    std::shared_ptr<MessageProcessor> getProcessor() const {return mProcessor.lock();}
    int64_t getTimer() const {return mTimer;}
    std::string toString() const override;
    std::string toStringAt(int64_t now) const;
    static int64_t currentTimeMillis();
};
}
