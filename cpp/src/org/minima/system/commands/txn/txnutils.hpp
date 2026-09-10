#pragma once
#include "org/minima/database/userprefs/txndb/TxnRow.hpp"
#include "org/minima/system/brains/TxPoWMiner.hpp"
namespace minima::commands {
class txnutils {
public:
    // Main publishes its current miner here when native startup is wired.
    // An absent Main fails at the same lookup point; it is not a no-op miner.
    static std::function<std::shared_ptr<TxPoWMiner>()> getMiner;
    static void setMMRandScripts(Transaction& transaction, Witness& witness, bool exitOnFail=true);
    static void setMMRandScripts(const Coin& coin, Witness& witness);
    static std::shared_ptr<TxnRow> createBurnTransaction(const std::vector<std::string>& excludeCoins,
        const MiniData& linkTransactionID, const MiniNumber& amount, bool sign=true);
};
}
