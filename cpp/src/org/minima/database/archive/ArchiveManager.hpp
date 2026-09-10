#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/objects/TxBlock.hpp"
#include "org/minima/database/cascade/Cascade.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <atomic>
namespace minima {
class ArchiveManager:public SqlDB {
public:
    using Blocks=std::vector<std::shared_ptr<TxBlock>>;
    std::atomic<int64_t> MAX_KEEP_BLOCKS;
    ArchiveManager(SqlDBServices services,std::function<bool()> shuttingDown,
                   std::function<int64_t()> clock=TimerMessage::currentTimeMillis);
    virtual int32_t getSize();
    virtual void checkCascadeRequired(const Cascade* cascade);
    void checkCascadeRequired(const Cascade& cascade){checkCascadeRequired(&cascade);}
    virtual std::shared_ptr<Cascade> loadCascade();
    virtual bool saveBlock(const TxBlock* block);
    bool saveBlock(const TxBlock& block){return saveBlock(&block);}
    virtual std::shared_ptr<TxBlock> loadBlock(const std::optional<std::string>& id);
    virtual std::shared_ptr<TxBlock> loadFirstBlock();
    virtual std::shared_ptr<TxBlock> loadLastBlock();
    virtual Blocks loadSyncBlockRange(const MiniNumber* start);
    Blocks loadSyncBlockRange(const MiniNumber& start){return loadSyncBlockRange(&start);}
    virtual MiniNumber exists(const std::optional<std::string>& id);
    virtual Blocks loadBlockRange(const MiniNumber* start,const MiniNumber* end);
    virtual Blocks loadBlockRange(const MiniNumber* start,const MiniNumber* end,bool descending);
    Blocks loadBlockRange(const MiniNumber& start,const MiniNumber& end){return loadBlockRange(&start,&end);}
    Blocks loadBlockRange(const MiniNumber& start,const MiniNumber& end,bool descending){return loadBlockRange(&start,&end,descending);}
    virtual std::shared_ptr<TxBlock> loadBlockFromNumber(const MiniNumber* block);
    std::shared_ptr<TxBlock> loadBlockFromNumber(const MiniNumber& block){return loadBlockFromNumber(&block);}
    virtual int32_t checkForCleanDB();
    virtual void hackShut();
    virtual void closeAndReopen();
protected:
    void createSQL() override;
private:
    std::function<bool()> mShuttingDown,mStartupDebug;
    std::function<int64_t()> mClock;
    std::shared_ptr<SqlPreparedStatement> SQL_INSERT_SYNCBLOCK,SQL_FIND_SYNCBLOCK,SQL_EXISTS_SYNCBLOCK,SQL_TOTAL_COUNT;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_RANGE,SQL_SELECT_FROMBBLOCK,SQL_DELETE_TXBLOCKS,SQL_SELECT_LAST,SQL_SELECT_FIRST;
    std::shared_ptr<SqlPreparedStatement> SQL_SELECT_LAST_BLOCK,SQL_SELECT_FIRST_BLOCK,SQL_SELECT_BLOCK,SQL_SELECT_SYNC_LIST,SAVE_CASCADE,LOAD_CASCADE;
    bool saveCascade(const Cascade& cascade);
    bool intSaveBlock(const TxBlock* block);
    int32_t cleanDB();
    std::shared_ptr<TxBlock> readBlock(SqlResultSet& rows);
    std::shared_ptr<TxBlock> loadEdge(const std::shared_ptr<SqlPreparedStatement>& statement);
    static void sort(Blocks& blocks,bool descending);
};
}
