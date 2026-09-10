#pragma once
#include "../Command.hpp"
namespace minima::commands {
class magic:public Command {
public:
    magic();
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
