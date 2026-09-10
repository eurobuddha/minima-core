#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class printmmr:public Command {
public:
    printmmr();
    std::optional<std::string> getFullHelp() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
