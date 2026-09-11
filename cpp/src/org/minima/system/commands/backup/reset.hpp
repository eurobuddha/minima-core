#pragma once
#include "../Command.hpp"
namespace minima::commands {
class reset:public Command {
public:
    reset();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action","archivefile","file","password","phrase","keys","keyuses"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<reset>();}
};
}
