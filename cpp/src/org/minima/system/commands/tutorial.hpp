#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class tutorial:public Command {
public:
    tutorial();
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<tutorial>();}
};
}
