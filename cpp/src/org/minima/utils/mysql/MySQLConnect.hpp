#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/database/cascade/Cascade.hpp"
#include "org/minima/objects/TxBlock.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
struct MySQLConnectServices {
    std::function<std::shared_ptr<SqlConnection>(const std::string&,const std::optional<std::string>&,const std::optional<std::string>&)> connect;
    std::function<void(const std::string&)> log;
    std::function<int64_t()> now=TimerMessage::currentTimeMillis;
};
class MySQLConnect {
public:
    using Text=std::optional<std::string>;
    static constexpr int MAX_SYNCBLOCKS=250;
    static MySQLConnectServices runtimeServices;
    MySQLConnect(Text host,Text database,Text username,Text password,bool readOnly=false,MySQLConnectServices services=runtimeServices);
    void init();
    void shutdown();
    void wipeAll();
    void wipeCoinsDB();
    bool saveCascade(const Cascade* cascade);
    bool saveCascade(const Cascade& cascade){return saveCascade(&cascade);}
    std::shared_ptr<Cascade> loadCascade();
    int32_t getCount();
    bool saveBlock(const TxBlock* block);
    bool saveBlock(const TxBlock& block){return saveBlock(&block);}
    std::shared_ptr<TxBlock> loadBlockFromID(const Text& id);
    std::shared_ptr<TxBlock> loadBlockFromNum(int64_t number);
    int64_t loadFirstBlock();
    int64_t loadLastBlock();
    std::vector<std::shared_ptr<TxBlock>> loadBlockRange(const MiniNumber* start);
    std::vector<std::shared_ptr<TxBlock>> loadBlockRange(const MiniNumber& start){return loadBlockRange(&start);}
    void insertCoin(const Coin* coin,int64_t blockSpent,const Text& date);
    void insertCoin(const Coin& coin,int64_t blockSpent,const Text& date){insertCoin(&coin,blockSpent,date);}
    int64_t getMaxCoinBlock();
    int64_t getTotalCoins();
    json::JSONObject searchCoins(const Text& query,bool hideToken);
    bool saveTxPoW(const TxPoW* txpow);
    bool saveTxPoW(const TxPoW& txpow){return saveTxPoW(&txpow);}
    std::shared_ptr<TxPoW> getTxPoW(const Text& id);
    static void main(const std::vector<std::string>& args);
private:
    Text mMySQLHost,mDatabase,mUsername,mPassword;
    std::shared_ptr<SqlConnection> mConnection;
    std::shared_ptr<SqlPreparedStatement> SQL_INSERT_SYNCBLOCK,SQL_FIND_SYNCBLOCK_ID,SQL_FIND_SYNCBLOCK_NUM,SQL_SELECT_RANGE,SQL_SELECT_LAST_BLOCK,SQL_SELECT_FIRST_BLOCK,SQL_COUNT,SAVE_CASCADE,LOAD_CASCADE,SQL_INSERT_COIN,SQL_LATEST_COIN,SQL_TOTAL_COIN,SQL_INSERT_TXPOW,SQL_GET_TXPOW;
    bool mReadOnly,DEBUG=false;
    MySQLConnectServices mServices;
    std::recursive_mutex mMonitor;
    void log(const std::string& message) const;
    std::shared_ptr<TxBlock> readBlock(SqlResultSet& rows);
    int64_t edge(const std::shared_ptr<SqlPreparedStatement>& statement);
    friend class MySQLConnectFixture;
};
}
