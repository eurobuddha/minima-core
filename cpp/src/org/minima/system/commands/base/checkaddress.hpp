#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class checkaddress:public Command {
public:
    checkaddress();
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
