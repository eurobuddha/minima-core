#pragma once
#include "org/minima/system/commands/Command.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
namespace minima::commands {
class systemcheck:public Command {
public:
    systemcheck();
    std::vector<std::string> getValidParams() const override{return {"processor","action"};}
    json::JSONValue runCommand() override;
    void printDetails(const std::shared_ptr<MessageProcessor>& processor);
    json::JSONObject getInfo(const std::shared_ptr<MessageProcessor>& processor);
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<systemcheck>();}
};
}
