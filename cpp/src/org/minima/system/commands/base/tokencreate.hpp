#pragma once
#include "../Command.hpp"
namespace minima::commands {
class tokencreate:public Command {
public:
    tokencreate();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<tokencreate>();}
};
}
