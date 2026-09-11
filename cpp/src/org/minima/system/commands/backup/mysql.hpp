#pragma once
#include "../Command.hpp"
namespace minima::commands {
class mysql:public Command {
public:
    mysql();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<mysql>();}
    static void convertMySQLParams(const std::optional<std::string>& details);
    static void main(const std::vector<std::string>& args);
};
}
