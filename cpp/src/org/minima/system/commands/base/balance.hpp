#pragma once
#include "../Command.hpp"
namespace minima::commands {
class balance:public Command {
public:
    balance();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
