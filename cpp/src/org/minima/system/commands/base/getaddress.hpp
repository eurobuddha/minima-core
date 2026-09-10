#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class getaddress:public Command {
public:
    getaddress();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
