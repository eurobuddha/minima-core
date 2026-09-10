#pragma once
#include "../Command.hpp"
namespace minima::commands {
class message:public Command {
public:
    message();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"uid","data"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<message>();}
};
}
