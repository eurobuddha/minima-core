#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class block:public Command {
public:
    block();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
