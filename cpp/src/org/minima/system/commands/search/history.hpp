#pragma once
#include "../Command.hpp"
#include "org/minima/objects/TxPoW.hpp"
namespace minima::commands {
class history:public Command {
public:
    history();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    virtual std::shared_ptr<json::JSONObject> getTxnDetails(TxPoW* txpow);
    std::shared_ptr<Command> getFunction() const override;
};
}
