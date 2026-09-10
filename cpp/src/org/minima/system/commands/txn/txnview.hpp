#pragma once
#include "../Command.hpp"
namespace minima::commands {
class txnview:public Command {
public:
    txnview();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
