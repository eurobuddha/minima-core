#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands::base {
class tutorial:public Command {
public:
    tutorial();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<tutorial>();}
};
}
