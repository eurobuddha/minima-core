#pragma once
#include "../Command.hpp"
namespace minima::commands {
class sendsign:public Command {
public:
    sendsign();
    static void main(const std::vector<std::optional<std::string>>* args=nullptr);
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
