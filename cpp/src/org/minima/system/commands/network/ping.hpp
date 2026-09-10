#pragma once
#include "../Command.hpp"
namespace minima::commands {
class ping:public Command {
public:
    ping();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"host"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<ping>();}
};
}
