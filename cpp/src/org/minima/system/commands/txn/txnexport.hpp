#pragma once
#include "../Command.hpp"
namespace minima::commands {
class txnexport:public Command {
public:
    txnexport();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
