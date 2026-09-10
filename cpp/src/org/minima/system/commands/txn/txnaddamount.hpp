#pragma once
#include "../Command.hpp"
namespace minima::commands {
class txnaddamount:public Command {
public:
    txnaddamount();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
    static std::vector<std::string> LOCKED_COINS;
    static bool LOCKED_COINS_ENABLED;
    static void enableCoinLock(bool enabled);
    static bool isCoinLockEnabled();
    static void addCoinLock(const std::string& id);
    static bool isCoinLocked(const std::string& id);
    static void clearCoinLocked();
};
}
