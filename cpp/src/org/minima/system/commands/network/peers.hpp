#pragma once
#include "../Command.hpp"
namespace minima::commands {
class peers:public Command {
public:
    peers();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"action","peerslist","max","file","url"};}
    json::JSONValue runCommand() override;
    static std::string getPeersList(int32_t max);
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<peers>();}
};
}
