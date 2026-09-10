#pragma once
#include "../Command.hpp"
namespace minima::commands {
class trace:public Command {
public:
    trace();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"enable","filter","network"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<trace>();}
};
}
