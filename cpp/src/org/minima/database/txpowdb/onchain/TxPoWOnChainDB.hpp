#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <atomic>

namespace minima {
class TxPoWOnChainDB : public SqlDB {
public:
    // Pinned Java evaluates the product as int before widening to long.
    inline static std::atomic<int64_t> MAX_ONCHAINSQL_MILLI{500654080};
    explicit TxPoWOnChainDB(SqlDBServices services,
        std::function<int64_t()> clock = TimerMessage::currentTimeMillis);
    virtual void wipeDB();
    virtual bool addOnChainTxPoW(const std::optional<std::string>& blockID,
        const MiniNumber* block, const std::optional<std::string>& txPoWID);
    bool addOnChainTxPoW(const std::optional<std::string>& blockID,
        const MiniNumber& block, const std::optional<std::string>& txPoWID) {
        return addOnChainTxPoW(blockID, &block, txPoWID);
    }
    virtual json::JSONObject getOnChainTxPoW(const std::optional<std::string>& txPoWID);
    virtual json::JSONArray getInBlockTxPoW(const std::optional<std::string>& blockID);
    virtual json::JSONArray getInBlockTxPoW(int64_t block);
    virtual json::JSONObject getFirstTxPoW();
    virtual json::JSONObject getLastTxPoW();
    virtual int32_t getSize();
    virtual int32_t cleanDB();
    virtual int32_t cleanDB(bool hard);
protected:
    void createSQL() override;
private:
    std::function<int64_t()> mClock;
    std::shared_ptr<SqlPreparedStatement> SQL_INSERT_ONCHAINTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_ONCHAINTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_DELETE_ONCHAINTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_ONBLOCKTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_ONBLOCKNUMTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_FIRSTTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_LASTTXPOW;
    std::shared_ptr<SqlPreparedStatement> SQL_TOTAL_TXPOW;
    json::JSONArray getInBlockRows(const std::shared_ptr<SqlPreparedStatement>& statement,
        const std::optional<std::string>& blockID, int64_t block, bool numeric);
    json::JSONObject getEndTxPoW(const std::shared_ptr<SqlPreparedStatement>& statement);
};
}
