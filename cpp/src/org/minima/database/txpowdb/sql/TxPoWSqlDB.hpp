#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <atomic>
namespace minima {
class TxPoWSqlDB:public SqlDB {
public:
    // Java main uses ambient user.home and DriverManager; native callers supply
    // the existing SQL provider and home value explicitly.
    static void main(const std::vector<std::string>* args,SqlDBServices services,const File& userHome);
    // Function-local storage preserves Java class initialization at first use.
    static std::atomic<int64_t>& maxSQLMilli();
    inline static MiniNumber MAX_RELEVANT_TXPOW{100};
    inline static std::atomic<bool> LOG_ADD_BLOCKS{false}, LOG_ADD_TRANSACTION{false};
    using TxPoWs=std::vector<std::shared_ptr<TxPoW>>;
    explicit TxPoWSqlDB(SqlDBServices services,std::function<int64_t()> clock=TimerMessage::currentTimeMillis);
    virtual void wipeDB();
    virtual int32_t customSizeQuery(const std::optional<std::string>& where);
    virtual bool addTxPoW(const TxPoW* txpow,bool relevant);
    bool addTxPoW(const TxPoW& txpow,bool relevant){return addTxPoW(&txpow,relevant);}
    virtual std::shared_ptr<TxPoW> getTxPoW(const std::optional<std::string>& id);
    virtual std::vector<std::optional<std::string>> getChildBlocks(const std::optional<std::string>& parent);
    virtual int32_t getRelevantSize();
    virtual TxPoWs getAllRelevant(int32_t limit){return getAllRelevant(limit,0);}
    virtual TxPoWs getAllRelevant(int32_t limit,int32_t offset);
    virtual TxPoWs getLatestTxPoW(int32_t limit,int32_t offset);
    virtual int32_t getLatestTxPoWSize(int64_t minTime);
    virtual int32_t getSize();
    virtual bool exists(const std::optional<std::string>& id);
    virtual int32_t cleanDB();
    virtual int32_t cleanDB(bool hard);
    virtual void closeAndReopen();
protected:
    void createSQL() override;
private:
    std::function<int64_t()> mClock;
    std::shared_ptr<SqlPreparedStatement> SQL_INSERT_TXPOW,SQL_SELECT_TXPOW,SQL_SELECT_CHILDREN,SQL_TOTAL_TXPOW,SQL_DELETE_TXPOW,SQL_EXISTS;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_TOPTXPOW,SQL_SELECT_TOPTXPOW_SIZE,SQL_SELECT_RELEVANT,SQL_SELECT_RELEVANTSIZE;
    [[maybe_unused]] std::shared_ptr<SqlPreparedStatement> SQL_SELECT_OLDEST;
    std::shared_ptr<TxPoW> readTxPoW(SqlResultSet& rows);
    TxPoWs queryTxPoWs(const std::shared_ptr<SqlPreparedStatement>& statement,int32_t limit,int32_t offset);
    int32_t querySize(const std::shared_ptr<SqlPreparedStatement>& statement);
};
}
