#pragma once
#include "../Command.hpp"
namespace minima::commands {
class p2pstate:public Command {
public:
    p2pstate();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<p2pstate>();}
};
}
