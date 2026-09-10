#pragma once
#include "../Command.hpp"
namespace minima::commands {
class quit:public Command {
public:
    quit();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"compact"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<quit>();}
};
}
