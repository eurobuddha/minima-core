#pragma once
#include "../Command.hpp"
namespace minima::commands {
class network:public Command {
public:
    network();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<network>();}
};
}
