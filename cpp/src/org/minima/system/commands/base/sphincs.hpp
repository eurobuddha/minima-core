#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima {class Coin;}
namespace minima::commands {
class sphincs:public Command {
    static std::string getOutCoinString(const Coin& coin);
    static json::JSONObject runCommand(const std::string& command);
public:
    sphincs();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
