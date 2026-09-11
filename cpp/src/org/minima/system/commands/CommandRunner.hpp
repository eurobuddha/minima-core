#pragma once
#include "Command.hpp"
namespace minima {
class CommandRunner {
    static std::vector<std::string> splitStringJSON(bool forceNormal,const std::string& input);
public:
    // This registry grows as concrete command classes are ported. Missing native
    // commands are not represented by fake implementations.
    static std::vector<std::shared_ptr<Command>> ALL_COMMANDS;
    static CommandRunner getRunner(){return CommandRunner();}
    json::JSONArray runMultiCommand(const std::string& command);
    json::JSONArray runMultiCommand(const std::string& miniDappID,const std::string& command);
    json::JSONArray runMultiCommandNullableUser(const std::optional<std::string>& miniDappID,const std::string& command);
    json::JSONObject runSingleCommand(const std::string& command);
    std::shared_ptr<Command> getCommandOnly(const std::string& name);
    std::shared_ptr<Command> getCommand(const std::string& command);
    std::optional<std::vector<std::string>> splitterQuotedPattern(const std::string& input);
    bool isCommandAllowed(const std::string& command);
};
}
