#pragma once
#include "../Command.hpp"
namespace minima::commands {
class coinimport:public Command {
public:
    coinimport();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"data","track"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<coinimport>();}
};
}
