#pragma once
#include "../../Command.hpp"
namespace minima::commands {
class rawfrom:public Command {
public:
    rawfrom();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    virtual json::JSONObject runCommand(const std::string& command);
    std::shared_ptr<Command> getFunction() const override;
};
}
