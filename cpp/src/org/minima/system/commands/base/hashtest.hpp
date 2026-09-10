#pragma once
#include "../Command.hpp"
namespace minima::commands {
class hashtest:public Command {
public:
    hashtest();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"amount"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<hashtest>();}
};
}
