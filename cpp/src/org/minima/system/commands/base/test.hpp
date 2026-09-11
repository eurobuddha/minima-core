#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class test:public Command {
public:
    test();
    std::vector<std::string> getValidParams() const override{return {"show","action"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<test>();}
    static void main();
};
}
