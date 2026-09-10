#pragma once
#include "../Command.hpp"
#include "org/minima/database/userprefs/txndb/TxnRow.hpp"
namespace minima::commands {
class txnauto:public Command {
public:
    txnauto();
    std::vector<std::string> getValidParams() const override{return {"id","amount","address","tokenid","sign","burn","mmrscript"};}
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override{return std::make_shared<txnauto>();}
    static std::shared_ptr<TxnRow> createTransaction(const std::optional<std::string>& address,const MiniNumber* amount);
    static std::shared_ptr<TxnRow> createTransaction(const std::optional<std::string>& address,const MiniNumber& amount){return createTransaction(address,&amount);}
};
}
