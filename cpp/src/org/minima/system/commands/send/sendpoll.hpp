#pragma once
#include "../Command.hpp"
#include "support/JavaExceptions.hpp"
#include "../sendpoll/SendPollManager.hpp"
namespace minima::commands {
class sendpoll:public Command {
public:
    // Main.getInstance().getSendPoll(): bound by the native owner when available.
    inline static std::function<std::shared_ptr<SendPollManager>()> getManager=[]()->std::shared_ptr<SendPollManager>{throw JavaNullPointerException("null Main receiver");};
    sendpoll();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
