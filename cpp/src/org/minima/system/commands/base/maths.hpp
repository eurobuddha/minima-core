#pragma once
#include "../Command.hpp"
#include "org/minima/objects/mmr/MMRData.hpp"
namespace minima::commands {
class maths:public Command {
public:
    maths();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
