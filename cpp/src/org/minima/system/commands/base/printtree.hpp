#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class printtree:public Command {
public:
    printtree();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    std::vector<std::string> getValidParams() const override;
};
}
