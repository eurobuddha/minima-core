#pragma once
#include "../Command.hpp"
#include "org/minima/utils/messages/Message.hpp"
namespace minima::commands {
class txnminepost:public Command {
public:
    txnminepost();
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    // Main binds this receiver when native startup is wired.
    static std::function<void(std::shared_ptr<Message>)> postMessage;
};
}
