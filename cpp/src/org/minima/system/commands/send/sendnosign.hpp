#pragma once
#include "../Command.hpp"
#include "org/minima/objects/Coin.hpp"
namespace minima::commands {
class sendnosign:public Command {
public:
    sendnosign();
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
};
}
