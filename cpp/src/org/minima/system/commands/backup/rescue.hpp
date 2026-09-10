#pragma once
#include "../Command.hpp"
namespace minima::commands {
class rescue:public Command {
public:
    rescue();
    std::vector<std::string> getValidParams() const override{return {"host"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<rescue>();}
};
}
