#pragma once
#include "../Command.hpp"
#include "org/minima/utils/messages/Message.hpp"
namespace minima::commands {
class connect : public Command {
public:
    // Bind to Main.getInstance().getNIOManager().PostMessage in the runtime.
    static std::function<void(std::shared_ptr<Message>)> postMessage;
    connect();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"host"};}
    json::JSONValue runCommand() override;
    static std::shared_ptr<Message> createConnectMessage(const std::optional<std::string>& fullHost);
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<connect>();}
};
}
