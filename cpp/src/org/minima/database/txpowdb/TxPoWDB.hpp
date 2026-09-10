#pragma once
#include "ram/RamDB.hpp"
#include "sql/TxPoWSqlDB.hpp"
#include "onchain/TxPoWOnChainDB.hpp"
#include "org/minima/utils/messages/Message.hpp"
#include "support/ChainServices.hpp"
namespace minima {
struct TxPoWDBServices {
    SqlDBServices sql;
    std::function<void(std::shared_ptr<Message>)> postMainMessage;
    // Resolve MinimaDB.getDB().getWallet() at each source call. The returned
    // wallet callbacks must resolve the actual wallet, including failures.
    std::function<ChainServices()> getWallet;
};
class TxPoWDB {
public:
    using TxPoWs=std::vector<std::shared_ptr<TxPoW>>;
    using IDs=std::vector<std::optional<std::string>>;
    explicit TxPoWDB(TxPoWDBServices services,
        std::function<int64_t()> sqlClock=TimerMessage::currentTimeMillis);
    virtual ~TxPoWDB()=default;
    virtual void loadSQLDB(const SqlDB::File& file);
    virtual void hardCloseSQLDB();
    virtual void saveDB(bool compact);
    virtual bool addTxPoW(std::shared_ptr<TxPoW> txpow);
    virtual void addSQLTxPoW(std::shared_ptr<TxPoW> txpow);
    virtual std::shared_ptr<TxPoW> getTxPoW(const std::optional<std::string>& id);
    virtual TxPoWs getAllTxPoW(const IDs* ids);
    TxPoWs getAllTxPoW(const IDs& ids){return getAllTxPoW(&ids);}
    virtual bool exists(const std::optional<std::string>& id);
    virtual TxPoWs getChildBlocks(const std::optional<std::string>& parent);
    virtual int32_t getRamSize();
    virtual int32_t getSqlSize();
    virtual SqlDB::File getSqlFile();
    virtual std::shared_ptr<TxPoWSqlDB> getSQLDB(){return mSqlDB;}
    virtual std::shared_ptr<TxPoWOnChainDB> getOnChainDB(){return mOnChainDB;}
    virtual void cleanDBRAM();
    virtual void wipeDBRAM();
    virtual void cleanDBSQL();
    virtual void clearMainChainTxns();
    virtual void setOnMainChain(const std::optional<std::string>& id);
    virtual void setInCascade(const std::optional<std::string>& id);
    virtual TxPoWs getAllUnusedTxns();
    virtual void removeMemPoolTxPoW(const std::optional<std::string>& id);
    virtual bool checkMempoolCoins(const MiniData& id);
    virtual std::shared_ptr<RamDB::Map> getCompleteMemPool();
protected:
    std::shared_ptr<RamDB> mRamDB;
    std::shared_ptr<TxPoWSqlDB> mSqlDB;
    std::shared_ptr<TxPoWOnChainDB> mOnChainDB;
private:
    TxPoWDBServices mServices;
    void log(const std::string& text);
};
}
