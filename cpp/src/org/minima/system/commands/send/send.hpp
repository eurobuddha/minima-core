#pragma once
#include "../Command.hpp"
#include "org/minima/objects/Coin.hpp"
namespace minima::commands {
class send:public Command {
public:
    send();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    class AddressAmount {
        MiniData mAddress;
        MiniNumber mAmount;
    public:
        AddressAmount(MiniData address,MiniNumber amount):mAddress(std::move(address)),mAmount(std::move(amount)){}
        const MiniData& getAddress() const {return mAddress;}
        const MiniNumber& getAmount() const {return mAmount;}
    };
    static std::vector<Coin> orderCoins(std::vector<Coin>& coins);
    static std::vector<Coin> selectCoins(std::vector<Coin>& coins,const MiniNumber& required,bool debug=false);
};
}
