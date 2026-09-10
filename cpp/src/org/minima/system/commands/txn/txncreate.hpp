#pragma once
#include "../Command.hpp"
namespace minima::commands {
class txncreate:public Command {
public:
    txncreate();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
