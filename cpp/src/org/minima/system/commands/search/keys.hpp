#pragma once
#include "../Command.hpp"
#include "org/minima/utils/messages/Message.hpp"
namespace minima::commands {
class keys:public Command {
public:
    keys();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    static bool checkKey(const std::optional<std::string>& publicKey);
    // Native Main binds this receiver; absent startup must not report success.
    static std::function<void(std::shared_ptr<Message>)> postMessage;
};
}
