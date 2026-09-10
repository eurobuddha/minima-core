#pragma once
#include "../Command.hpp"
namespace minima::commands {
class status:public Command {
public:
    status();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"clean","debug","complete"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<status>();}
    static void main(const std::vector<std::string>& args);
};
}
