#pragma once
#include "../Command.hpp"
namespace minima::commands {
class logs:public Command {
public:
    logs();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"scripts","mining","maxima","blocks","networking","ibd","peerschecker","txpowdb"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<logs>();}
};
}
