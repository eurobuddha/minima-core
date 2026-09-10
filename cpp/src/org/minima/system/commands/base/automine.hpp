#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class automine:public Command {
public:
    automine();
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
