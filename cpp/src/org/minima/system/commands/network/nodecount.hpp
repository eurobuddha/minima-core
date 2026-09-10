#pragma once
#include "../Command.hpp"
namespace minima::commands {
class nodecount:public Command {
public:
    nodecount();
    std::vector<std::string> getValidParams() const override{return {"file","complete"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<nodecount>();}
};
}
