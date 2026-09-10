#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class convert:public Command {
public:
    convert();
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    std::optional<std::string> getFullHelp() const override;
};
}
