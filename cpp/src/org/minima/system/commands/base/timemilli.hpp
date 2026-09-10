#pragma once
#include "../Command.hpp"
namespace minima::commands {
class timemilli:public Command {
public:
    timemilli();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"minutesback","hoursback"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<timemilli>();}
};
}
