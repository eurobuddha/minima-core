#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/objects/Coin.hpp"

namespace minima {
class CoinDB : public SqlDB {
public:
    using Coins = std::vector<std::shared_ptr<Coin>>;
    static void createCoinDB(const File& file, SqlDBServices services);
    static std::shared_ptr<CoinDB> getTxPoWTreeCoinDB();
    virtual void closeAndReopen();
    virtual bool insertCoin(const MiniData* treeID, const Coin* coin, const MiniNumber* block);
    bool insertCoin(const MiniData& treeID, const Coin& coin, const MiniNumber& block) {
        return insertCoin(&treeID, &coin, &block);
    }
    virtual Coins getAllCoins(const MiniData* treeID);
    Coins getAllCoins(const MiniData& treeID) { return getAllCoins(&treeID); }
    virtual void clearOldCoins(int64_t minimumBlock);
    virtual int32_t getSize();
protected:
    void createSQL() override;
private:
    CoinDB(const File& file, SqlDBServices services);
    static std::shared_ptr<CoinDB> mCoinDB;
    static std::mutex mInstanceLock;
    std::shared_ptr<SqlPreparedStatement> SQL_INSERT_COIN, SQL_SELECT_ALLCOINS,
        SQL_CLEAR_COINS, SQL_SIZE;
};
}
