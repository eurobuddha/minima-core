#include "ArchiveManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/JavaIOExceptionText.hpp"
#include <algorithm>
#include <bit>
namespace minima {
namespace {
template<class T> const T& required(const T* value){if(!value)throw SqlRuntimeException("java.lang.NullPointerException","null archive object");return *value;}
const MiniNumber& tipHeight(const Cascade& cascade){return sqlRequired(cascade.getTip()).getTxPoW().getBlockNumber();}
}
ArchiveManager::ArchiveManager(SqlDBServices services,std::function<bool()> shuttingDown,std::function<int64_t()> clock)
    :SqlDB(services),MAX_KEEP_BLOCKS(std::bit_cast<int64_t>(uint64_t(2000)*uint64_t(GeneralParams::NUMBER_DAYS_ARCHIVE))),
     mShuttingDown(std::move(shuttingDown)),mStartupDebug(std::move(services.startupDebugLogs)),mClock(std::move(clock)){
    if(!mShuttingDown||!mStartupDebug||!mClock)throw std::invalid_argument("ArchiveManager requires shutdown, startup-debug and clock services");
}
void ArchiveManager::createSQL(){
    if(mStartupDebug())log("Create ArchiveDB..");
    auto statement=sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `syncblock` ("
        "  `id` bigint auto_increment,"
        "  `txpowid` varchar(80) NOT NULL UNIQUE,"
        "  `block` bigint NOT NULL UNIQUE,"
        "  `timemilli` bigint NOT NULL,"
        "  `syncdata` blob NOT NULL)");
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `cascadedata` ("
        "\t\t`id` int auto_increment,"
        "\t\t`cascadetip` BIGINT NOT NULL,"
        "\t\t`fulldata` blob NOT NULL)");
    sqlRequired(statement).execute("CREATE INDEX IF NOT EXISTS fastsearch ON syncblock ( txpowid, block )");
    sqlRequired(statement).close();
    if(mStartupDebug())log("Create ArchiveDB.. finish");
    SQL_INSERT_SYNCBLOCK=sqlRequired(mSQLConnection).prepareStatement("INSERT IGNORE INTO syncblock ( txpowid, block, timemilli, syncdata ) VALUES ( ?, ? ,? ,? )");
    SQL_FIND_SYNCBLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE txpowid=?");
    SQL_EXISTS_SYNCBLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT block FROM syncblock WHERE txpowid=?");
    SQL_TOTAL_COUNT=sqlRequired(mSQLConnection).prepareStatement("SELECT COUNT(*) as tot FROM syncblock");
    SQL_SELECT_RANGE=sqlRequired(mSQLConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE block>? AND block<?");
    SQL_SELECT_FROMBBLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE block=?");
    SQL_DELETE_TXBLOCKS=sqlRequired(mSQLConnection).prepareStatement("DELETE FROM syncblock WHERE block < ?");
    SQL_SELECT_LAST=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM syncblock ORDER BY block ASC LIMIT 1");
    SQL_SELECT_FIRST=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM syncblock ORDER BY block DESC LIMIT 1");
    SQL_SELECT_LAST_BLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT block FROM syncblock ORDER BY block ASC LIMIT 1");
    SQL_SELECT_FIRST_BLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT block FROM syncblock ORDER BY block DESC LIMIT 1");
    SQL_SELECT_BLOCK=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM syncblock WHERE block=?");
    SQL_SELECT_SYNC_LIST=sqlRequired(mSQLConnection).prepareStatement("SELECT syncdata FROM syncblock WHERE block<? AND block>=?");
    SAVE_CASCADE=sqlRequired(mSQLConnection).prepareStatement("INSERT INTO cascadedata ( cascadetip, fulldata ) VALUES ( ?, ? )");
    LOAD_CASCADE=sqlRequired(mSQLConnection).prepareStatement("SELECT fulldata FROM cascadedata ORDER BY cascadetip ASC LIMIT 1");
}
int32_t ArchiveManager::getSize(){
    std::lock_guard lock(mSQLLock);try{checkOpen();auto rows=sqlRequired(SQL_TOTAL_COUNT).executeQuery();if(sqlRequired(rows).next())return rows->getInt("tot");}
    catch(const SqlException& error){log(sqlExceptionText(error));}return -1;
}
void ArchiveManager::checkCascadeRequired(const Cascade* cascade){
    const auto& incoming=required(cascade);if(incoming.getLength()<=0)return;
    auto stored=loadCascade();
    if(!stored){
        if(!Cascade::checkCascadeCorrect(incoming,[this](const std::string& text){log(text);})){
            log("[!] INCONSISTENT Cascade to save in Archive.. not saving..");return;
        }
        log("Saving Cascade in ARCHIVEDB.. tip : "+tipHeight(incoming).toString());saveCascade(incoming);
    }else{
        auto block=loadLastBlock();
        if(block){
            auto start=block->getTxPoW().getBlockNumber();auto end=sqlRequired(loadFirstBlock()).getTxPoW().getBlockNumber();
            auto height=tipHeight(*stored);
            if(!(height.isMoreEqual(start.sub(MiniNumber(1)))&&height.isLessEqual(end))){
                log("RE-Saving Cascade in ARCHIVEDB.. tip : "+tipHeight(incoming).toString());saveCascade(incoming);
            }
        }else log("Cascade already in ARCHIVEDB.. tip : "+tipHeight(*stored).toString());
    }
}
bool ArchiveManager::saveCascade(const Cascade& cascade){
    executeGenericSQL("DELETE FROM cascadedata WHERE id>=0");
    std::optional<std::vector<uint8_t>> bytes;
    try{bytes=cascade.serialise();}catch(const wire::IOException& error){log(javaIOExceptionText(error));}
    sqlRequired(SAVE_CASCADE).clearParameters();sqlRequired(SAVE_CASCADE).setLong(1,tipHeight(cascade).getAsLong());
    if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null serialized Cascade");
    sqlRequired(SAVE_CASCADE).setBytes(2,bytes);sqlRequired(SAVE_CASCADE).execute();return true;
}
std::shared_ptr<Cascade> ArchiveManager::loadCascade(){
    std::lock_guard lock(mSQLLock);sqlRequired(LOAD_CASCADE).clearParameters();auto rows=sqlRequired(LOAD_CASCADE).executeQuery();
    if(sqlRequired(rows).next()){
        auto bytes=rows->getBytes("fulldata");if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null cascade bytes");
        auto cascade=Cascade::convertMiniDataVersion(MiniData(*bytes),[this](const wire::IOException& error){log(javaIOExceptionText(error));});
        if(cascade)return std::make_shared<Cascade>(std::move(*cascade));
    }return nullptr;
}
bool ArchiveManager::saveBlock(const TxBlock* block){
    std::lock_guard lock(mSQLLock);try{intSaveBlock(block);}catch(const std::exception&){intSaveBlock(block);}return true;
}
bool ArchiveManager::intSaveBlock(const TxBlock* block){
    std::lock_guard lock(mSQLLock);checkOpen();std::optional<std::vector<uint8_t>> bytes;
    try{bytes=required(block).serialise();}catch(const wire::IOException& error){log(javaIOExceptionText(error));}
    sqlRequired(SQL_INSERT_SYNCBLOCK).clearParameters();sqlRequired(SQL_INSERT_SYNCBLOCK).setString(1,block->getTxPoW().getTxPoWID());
    sqlRequired(SQL_INSERT_SYNCBLOCK).setLong(2,block->getTxPoW().getBlockNumber().getAsLong());sqlRequired(SQL_INSERT_SYNCBLOCK).setLong(3,mClock());
    if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null serialized TxBlock");
    sqlRequired(SQL_INSERT_SYNCBLOCK).setBytes(4,bytes);sqlRequired(SQL_INSERT_SYNCBLOCK).execute();return true;
}
std::shared_ptr<TxBlock> ArchiveManager::readBlock(SqlResultSet& rows){
    auto bytes=rows.getBytes("syncdata");if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null block bytes");
    auto block=TxBlock::convertMiniDataVersion(MiniData(*bytes),[this](const wire::IOException& error){log(javaIOExceptionText(error));});
    return block?std::make_shared<TxBlock>(std::move(*block)):nullptr;
}
std::shared_ptr<TxBlock> ArchiveManager::loadBlock(const std::optional<std::string>& id){
    std::lock_guard lock(mSQLLock);try{
        checkOpen();sqlRequired(SQL_FIND_SYNCBLOCK).clearParameters();sqlRequired(SQL_FIND_SYNCBLOCK).setString(1,id);
        auto rows=sqlRequired(SQL_FIND_SYNCBLOCK).executeQuery();if(sqlRequired(rows).next())return readBlock(*rows);
    }catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;
}
std::shared_ptr<TxBlock> ArchiveManager::loadEdge(const std::shared_ptr<SqlPreparedStatement>& statement){
    std::lock_guard lock(mSQLLock);try{
        // The reference observes replacement of this field during checkOpen.
        checkOpen();sqlRequired(statement).clearParameters();auto rows=sqlRequired(statement).executeQuery();int64_t height=-1;
        if(sqlRequired(rows).next())height=rows->getLong("block");
        if(height!=-1){sqlRequired(SQL_SELECT_BLOCK).clearParameters();sqlRequired(SQL_SELECT_BLOCK).setLong(1,height);
            rows=sqlRequired(SQL_SELECT_BLOCK).executeQuery();if(sqlRequired(rows).next())return readBlock(*rows);}
    }catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;
}
std::shared_ptr<TxBlock> ArchiveManager::loadFirstBlock(){return loadEdge(SQL_SELECT_FIRST_BLOCK);}
std::shared_ptr<TxBlock> ArchiveManager::loadLastBlock(){return loadEdge(SQL_SELECT_LAST_BLOCK);}
void ArchiveManager::sort(Blocks& blocks,bool descending){
    std::stable_sort(blocks.begin(),blocks.end(),[descending](const auto& a,const auto& b){
        const auto& left=sqlRequired(descending?b:a).getTxPoW().getBlockNumber();
        const auto& right=sqlRequired(descending?a:b).getTxPoW().getBlockNumber();return left.compareTo(right)<0;
    });
}
ArchiveManager::Blocks ArchiveManager::loadSyncBlockRange(const MiniNumber* start){
    std::lock_guard lock(mSQLLock);Blocks blocks;try{
        checkOpen();sqlRequired(SQL_SELECT_SYNC_LIST).clearParameters();sqlRequired(SQL_SELECT_SYNC_LIST).setLong(1,required(start).getAsLong());
        auto end=start->sub(MiniNumber(256));if(end.isLessEqual(MiniNumber(1)))end=MiniNumber(1);
        sqlRequired(SQL_SELECT_SYNC_LIST).setLong(2,end.getAsLong());auto rows=sqlRequired(SQL_SELECT_SYNC_LIST).executeQuery();
        while(sqlRequired(rows).next()){if(mShuttingDown())break;blocks.push_back(readBlock(*rows));}
    }catch(const SqlException& error){log(sqlExceptionText(error));}sort(blocks,true);return blocks;
}
MiniNumber ArchiveManager::exists(const std::optional<std::string>& id){
    std::lock_guard lock(mSQLLock);try{
        checkOpen();sqlRequired(SQL_EXISTS_SYNCBLOCK).clearParameters();sqlRequired(SQL_EXISTS_SYNCBLOCK).setString(1,id);
        auto rows=sqlRequired(SQL_EXISTS_SYNCBLOCK).executeQuery();if(sqlRequired(rows).next())return MiniNumber(rows->getLong("block"));
    }catch(const SqlException& error){log(sqlExceptionText(error));}return MiniNumber(-1);
}
ArchiveManager::Blocks ArchiveManager::loadBlockRange(const MiniNumber* start,const MiniNumber* end){
    std::lock_guard lock(mSQLLock);return loadBlockRange(start,end,true);
}
ArchiveManager::Blocks ArchiveManager::loadBlockRange(const MiniNumber* start,const MiniNumber* end,bool descending){
    std::lock_guard lock(mSQLLock);Blocks blocks;try{
        checkOpen();sqlRequired(SQL_SELECT_RANGE).clearParameters();sqlRequired(SQL_SELECT_RANGE).setLong(1,required(start).getAsLong());
        sqlRequired(SQL_SELECT_RANGE).setLong(2,required(end).getAsLong());auto rows=sqlRequired(SQL_SELECT_RANGE).executeQuery();
        while(sqlRequired(rows).next())blocks.push_back(readBlock(*rows));
    }catch(const SqlException& error){log(sqlExceptionText(error));}sort(blocks,descending);return blocks;
}
std::shared_ptr<TxBlock> ArchiveManager::loadBlockFromNumber(const MiniNumber* block){
    std::lock_guard lock(mSQLLock);try{
        checkOpen();sqlRequired(SQL_SELECT_FROMBBLOCK).clearParameters();sqlRequired(SQL_SELECT_FROMBBLOCK).setLong(1,required(block).getAsLong());
        auto rows=sqlRequired(SQL_SELECT_FROMBBLOCK).executeQuery();if(sqlRequired(rows).next())return readBlock(*rows);
    }catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;
}
int32_t ArchiveManager::checkForCleanDB(){return GeneralParams::ARCHIVE?0:cleanDB();}
int32_t ArchiveManager::cleanDB(){
    std::lock_guard lock(mSQLLock);try{
        checkOpen();sqlRequired(SQL_SELECT_FIRST_BLOCK).clearParameters();auto rows=sqlRequired(SQL_SELECT_FIRST_BLOCK).executeQuery();
        if(!sqlRequired(rows).next())return 0;auto block=rows->getLong("block");auto cutoff=MiniNumber(block).sub(MiniNumber(MAX_KEEP_BLOCKS.load()));
        sqlRequired(SQL_DELETE_TXBLOCKS).clearParameters();sqlRequired(SQL_DELETE_TXBLOCKS).setLong(1,cutoff.getAsLong());return sqlRequired(SQL_DELETE_TXBLOCKS).executeUpdate();
    }catch(const SqlException& error){log(sqlExceptionText(error));}return 0;
}
void ArchiveManager::hackShut(){sqlRequired(mSQLConnection).close();}
void ArchiveManager::closeAndReopen(){std::lock_guard lock(mSQLLock);try{saveDB(false);mSQLConnection.reset();checkOpen(false);}catch(const SqlException& error){log(sqlExceptionText(error));}}
}
