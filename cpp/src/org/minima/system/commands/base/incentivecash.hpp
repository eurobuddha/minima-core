#pragma once
#include "../Command.hpp"
namespace minima::commands {
class incentivecash:public Command {
public:
    incentivecash();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override{return {"uid"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<incentivecash>();}
protected:
    // Native transport boundary; the production default is the actual RPC client.
    virtual std::string requestPUT(const std::string& url);
};
}
