#pragma once
#include "../Command.hpp"
#include "org/minima/objects/TxPoW.hpp"
namespace minima::commands {
class scanchain:public Command {
public:
    scanchain();
    std::vector<std::string> getValidParams() const override{return {"depth","offset"};}
    json::JSONValue runCommand() override;
    virtual std::shared_ptr<json::JSONObject> getTransactionDetails(TxPoW* txpow);
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<scanchain>();}
};
}
