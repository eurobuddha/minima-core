#pragma once
#include "SendPollMessage.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
class SendPollManager : public MessageProcessor {
    std::mutex mSyncObject;
    std::vector<SendPollMessage> mSendCommands;
    std::function<json::JSONObject(const std::optional<std::string>&)> mRunCommand;
protected:
    SendPollManager(std::function<json::JSONObject(const std::optional<std::string>&)> runCommand,std::function<void()> releaseLocks);
    void processMessage(std::shared_ptr<Message> message) override;
    // Native timing seams allow offline tests without executing wallet commands.
    virtual void pauseBetweenCommands();
    virtual void schedulePoll();
public:
    static constexpr const char* SENDPOLL_FUNCTION="SENDPOLL_FUNCTION";
    inline static std::atomic<int64_t> SENDPOLL_TIMER{30000};
    // The Java source intentionally has these two string values transposed.
    static constexpr const char* SENDPOLL_LIST="SENDPOLL_CLEAR";
    static constexpr const char* SENDPOLL_CLEAR="SENDPOLL_LIST";
    static std::shared_ptr<SendPollManager> create(std::function<json::JSONObject(const std::optional<std::string>&)> runCommand,std::function<void()> releaseLocks);
    void addSendCommand(std::optional<std::string> command);
    std::vector<SendPollMessage> listCommands();
    void removeCommand(const std::optional<std::string>& uid);
};
}
