#pragma once
#include "../Command.hpp"
namespace minima::commands {
class healthcheck:public Command {
public:
    healthcheck();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<healthcheck>();}
};
}
