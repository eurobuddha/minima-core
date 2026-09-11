#pragma once
#include "Command.hpp"
namespace minima {
class CommandRunner {
    static std::vector<std::string> splitStringJSON(bool forceNormal,const std::string& input);
public:
    // The pinned public array is exposed as a stable mutable vector reference.
    // Lazy construction avoids C++ cross-translation-unit initialization order.
    static std::vector<std::shared_ptr<Command>>& ALL_COMMANDS();
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
