#pragma once
#include "org/minima/system/commands/Command.hpp"
namespace minima::commands {
class missingcmd:public Command {
    std::optional<std::string> mInput,mError;
public:
    missingcmd(std::optional<std::string> input,std::optional<std::string> error):Command("missingcmd",""),mInput(std::move(input)),mError(std::move(error)){}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
