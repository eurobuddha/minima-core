#pragma once
#include "../Command.hpp"
#include "org/minima/objects/TxPoW.hpp"
namespace minima::commands {
class txnpost:public Command {
public:
    txnpost();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    static std::shared_ptr<TxPoW> postTxn(const std::optional<std::string>& id,const MiniNumber& burn,bool automatic,bool mineSync);
};
}
