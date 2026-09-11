#include "support/JavaUnixFile.hpp"
#include <iostream>
#include "TxPoWSqlDB.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/JavaLowerCase.hpp"
#include <bit>
namespace minima {
namespace {
const std::string createTable="CREATE TABLE IF NOT EXISTS `txpow` ("
    "  `id` bigint auto_increment,"
    "  `txpowid` varchar(80) NOT NULL UNIQUE,"
    "  `isblock` tinyint NOT NULL,"
    "  `istransaction` tinyint NOT NULL,"
    "  `parentid` varchar(80) NOT NULL,"
    "  `timemilli` bigint NOT NULL,"
    "  `txpowdata` blob NOT NULL,"
    "  `isrelevant` tinyint NOT NULL"
    ")";
const TxPoW& requiredTxPoW(const TxPoW* txpow){if(!txpow)throw SqlRuntimeException("java.lang.NullPointerException","null TxPoW");return *txpow;}
void removeAll(std::string& text,const std::string& token){
    // Java replaceAll scans the original input once; deletion can create a new
    // occurrence spanning a removed match, which must survive this pass.
    std::string result;size_t start=0,pos;
    while((pos=text.find(token,start))!=std::string::npos){result.append(text,start,pos-start);start=pos+token.size();}
    result.append(text,start,std::string::npos);text=std::move(result);
}
}
std::atomic<int64_t>& TxPoWSqlDB::maxSQLMilli(){
    static std::atomic<int64_t> value{std::bit_cast<int64_t>(uint64_t(86400000)*uint64_t(GeneralParams::NUMBER_DAYS_SQLTXPOWDB))};return value;
}
TxPoWSqlDB::TxPoWSqlDB(SqlDBServices services,std::function<int64_t()> clock):SqlDB(std::move(services)),mClock(std::move(clock)){
    (void)maxSQLMilli();if(!mClock)throw std::invalid_argument("TxPoWSqlDB requires a clock");
}
void TxPoWSqlDB::createSQL(){
    auto statement=sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute(createTable);
    sqlRequired(statement).execute("CREATE INDEX IF NOT EXISTS fastsearch ON txpow ( txpowid, parentid )");
    sqlRequired(statement).close();
    SQL_INSERT_TXPOW=sqlRequired(mSQLConnection).prepareStatement("INSERT IGNORE INTO txpow ( txpowid, isblock, istransaction, parentid, timemilli, txpowdata, isrelevant ) VALUES ( ?, ? ,? ,? ,? ,? ,? )");
    SQL_SELECT_TXPOW=sqlRequired(mSQLConnection).prepareStatement("SELECT txpowdata FROM txpow WHERE txpowid=?");
    SQL_SELECT_CHILDREN=sqlRequired(mSQLConnection).prepareStatement("SELECT txpowid FROM txpow WHERE isblock=1 AND parentid=?");
    SQL_TOTAL_TXPOW=sqlRequired(mSQLConnection).prepareStatement("SELECT COUNT(*) AS tot FROM txpow");
    SQL_DELETE_TXPOW=sqlRequired(mSQLConnection).prepareStatement("DELETE FROM txpow WHERE timemilli < ? AND isrelevant=0");
    SQL_EXISTS=sqlRequired(mSQLConnection).prepareStatement("SELECT txpowid FROM txpow WHERE txpowid=?");
    SQL_SELECT_RELEVANT=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM txpow WHERE isrelevant=1 ORDER BY timemilli DESC LIMIT ? OFFSET ?");
    SQL_SELECT_RELEVANTSIZE=sqlRequired(mSQLConnection).prepareStatement("SELECT Count(*) AS tot FROM txpow WHERE isrelevant=1");
    SQL_SELECT_TOPTXPOW=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM txpow ORDER BY timemilli DESC LIMIT ? OFFSET ?");
    SQL_SELECT_TOPTXPOW_SIZE=sqlRequired(mSQLConnection).prepareStatement("SELECT Count(*) AS tot FROM txpow WHERE timemilli > ?");
}
void TxPoWSqlDB::wipeDB(){std::lock_guard lock(mSQLLock);checkOpen();auto statement=sqlRequired(mSQLConnection).createStatement();sqlRequired(statement).execute("DROP ALL OBJECTS");sqlRequired(statement).execute(createTable);sqlRequired(statement).close();}
int32_t TxPoWSqlDB::customSizeQuery(const std::optional<std::string>& condition){
    std::lock_guard lock(mSQLLock);
    try{
        checkOpen();if(!condition)throw SqlRuntimeException("java.lang.NullPointerException","null SQL condition");
        auto where=javaLowerCase(*condition);for(const std::string token:{";","update","delete","insert"})removeAll(where,token);
        auto statement=sqlRequired(mSQLConnection).createStatement();sqlRequired(statement).execute("SELECT Count(*) AS tot FROM txpow WHERE "+where);
        auto rows=sqlRequired(statement).getResultSet();if(sqlRequired(rows).next())return sqlRequired(rows).getInt("tot");
    }catch(const SqlException& error){log(sqlExceptionText(error));}
    return -1;
}
bool TxPoWSqlDB::addTxPoW(const TxPoW* txpow,bool relevant){
    std::lock_guard lock(mSQLLock);
    try{
        if(LOG_ADD_BLOCKS&&requiredTxPoW(txpow).isBlock())log("[+] BLOCK ADDED TO TXPOWDB : "+txpow->getBlockNumber().toString());
        if(LOG_ADD_TRANSACTION&&requiredTxPoW(txpow).isTransaction())log("[+] TRANSACTION ADDED TO TXPOWDB : "+txpow->getBlockNumber().toString()+" - "+txpow->getTransaction().toJSON().toString());
        checkOpen();std::optional<std::vector<uint8_t>> bytes;
        try{bytes=requiredTxPoW(txpow).serialise();}
        catch(const wire::IOException& error){log(javaIOExceptionText(error));}
        sqlRequired(SQL_INSERT_TXPOW).clearParameters();
        sqlRequired(SQL_INSERT_TXPOW).setString(1,txpow->getTxPoWID());
        sqlRequired(SQL_INSERT_TXPOW).setInt(2,txpow->isBlock()?1:0);
        sqlRequired(SQL_INSERT_TXPOW).setInt(3,txpow->isTransaction()?1:0);
        sqlRequired(SQL_INSERT_TXPOW).setString(4,txpow->getParentID().to0xString());
        sqlRequired(SQL_INSERT_TXPOW).setLong(5,mClock());
        if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null serialized TxPoW");
        sqlRequired(SQL_INSERT_TXPOW).setBytes(6,bytes);
        sqlRequired(SQL_INSERT_TXPOW).setInt(7,relevant?1:0);
        sqlRequired(SQL_INSERT_TXPOW).execute();return true;
    }catch(const SqlException& error){log(sqlExceptionText(error));}
    return false;
}
std::shared_ptr<TxPoW> TxPoWSqlDB::readTxPoW(SqlResultSet& rows){
    auto bytes=rows.getBytes("txpowdata");if(!bytes)throw SqlRuntimeException("java.lang.NullPointerException","null TxPoW bytes");
    auto result=TxPoW::convertMiniDataVersion(MiniData(*bytes),[this](const wire::IOException& error){
        log(javaIOExceptionText(error));
    });
    return result?std::make_shared<TxPoW>(std::move(*result)):nullptr;
}
std::shared_ptr<TxPoW> TxPoWSqlDB::getTxPoW(const std::optional<std::string>& id){
    std::lock_guard lock(mSQLLock);
    try{checkOpen();sqlRequired(SQL_SELECT_TXPOW).clearParameters();sqlRequired(SQL_SELECT_TXPOW).setString(1,id);auto rows=sqlRequired(SQL_SELECT_TXPOW).executeQuery();if(sqlRequired(rows).next())return readTxPoW(*rows);}
    catch(const SqlException& error){log(sqlExceptionText(error));}return nullptr;
}
std::vector<std::optional<std::string>> TxPoWSqlDB::getChildBlocks(const std::optional<std::string>& parent){
    std::lock_guard lock(mSQLLock);std::vector<std::optional<std::string>> result;
    try{checkOpen();sqlRequired(SQL_SELECT_CHILDREN).clearParameters();sqlRequired(SQL_SELECT_CHILDREN).setString(1,parent);auto rows=sqlRequired(SQL_SELECT_CHILDREN).executeQuery();while(sqlRequired(rows).next())result.push_back(rows->getString("txpowid"));}
    catch(const SqlException& error){log(sqlExceptionText(error));}return result;
}
int32_t TxPoWSqlDB::querySize(const std::shared_ptr<SqlPreparedStatement>& statement){
    std::lock_guard lock(mSQLLock);try{checkOpen();auto rows=sqlRequired(statement).executeQuery();if(sqlRequired(rows).next())return rows->getInt("tot");}
    catch(const SqlException& error){log(sqlExceptionText(error));}return -1;
}
int32_t TxPoWSqlDB::getRelevantSize(){return querySize(SQL_SELECT_RELEVANTSIZE);}
int32_t TxPoWSqlDB::getSize(){return querySize(SQL_TOTAL_TXPOW);}
TxPoWSqlDB::TxPoWs TxPoWSqlDB::queryTxPoWs(const std::shared_ptr<SqlPreparedStatement>& statement,int32_t limit,int32_t offset){
    std::lock_guard lock(mSQLLock);TxPoWs result;
    try{checkOpen();sqlRequired(statement).clearParameters();sqlRequired(statement).setInt(1,limit);sqlRequired(statement).setInt(2,offset);auto rows=sqlRequired(statement).executeQuery();while(sqlRequired(rows).next())result.push_back(readTxPoW(*rows));}
    catch(const SqlException& error){log(sqlExceptionText(error));}return result;
}
TxPoWSqlDB::TxPoWs TxPoWSqlDB::getAllRelevant(int32_t limit,int32_t offset){return queryTxPoWs(SQL_SELECT_RELEVANT,limit,offset);}
TxPoWSqlDB::TxPoWs TxPoWSqlDB::getLatestTxPoW(int32_t limit,int32_t offset){return queryTxPoWs(SQL_SELECT_TOPTXPOW,limit,offset);}
int32_t TxPoWSqlDB::getLatestTxPoWSize(int64_t minTime){
    std::lock_guard lock(mSQLLock);try{checkOpen();sqlRequired(SQL_SELECT_TOPTXPOW_SIZE).clearParameters();sqlRequired(SQL_SELECT_TOPTXPOW_SIZE).setLong(1,minTime);auto rows=sqlRequired(SQL_SELECT_TOPTXPOW_SIZE).executeQuery();if(sqlRequired(rows).next())return rows->getInt("tot");}
    catch(const SqlException& error){log(sqlExceptionText(error));}return -1;
}
bool TxPoWSqlDB::exists(const std::optional<std::string>& id){
    std::lock_guard lock(mSQLLock);try{checkOpen();sqlRequired(SQL_EXISTS).clearParameters();sqlRequired(SQL_EXISTS).setString(1,id);auto rows=sqlRequired(SQL_EXISTS).executeQuery();return sqlRequired(rows).next();}
    catch(const SqlException&){}return false;
}
int32_t TxPoWSqlDB::cleanDB(){std::lock_guard lock(mSQLLock);return cleanDB(false);}
int32_t TxPoWSqlDB::cleanDB(bool hard){
    std::lock_guard lock(mSQLLock);try{
        checkOpen();auto now=mClock();auto time=std::bit_cast<int64_t>(uint64_t(now)-uint64_t(maxSQLMilli().load()));
        if(hard){if(LOG_ADD_BLOCKS)log("[+] TxPoWDB wipe clean..");time=std::bit_cast<int64_t>(uint64_t(mClock())+100000u);}
        sqlRequired(SQL_DELETE_TXPOW).clearParameters();sqlRequired(SQL_DELETE_TXPOW).setLong(1,time);auto count=sqlRequired(SQL_DELETE_TXPOW).executeUpdate();
        if(LOG_ADD_BLOCKS)log("[+] TxPoWDB clean up deleted : "+std::to_string(count));return count;
    }catch(const SqlException& error){log(sqlExceptionText(error));}return 0;
}
void TxPoWSqlDB::closeAndReopen(){std::lock_guard lock(mSQLLock);try{saveDB(false);mSQLConnection.reset();checkOpen(false);}catch(const SqlException& error){log(sqlExceptionText(error));}}
void TxPoWSqlDB::main(const std::vector<std::string>*,SqlDBServices services,const File& userHome) {
    const auto file=userHome?javaUnixChildFile(*userHome,"testsql"):std::filesystem::path("testsql");
    TxPoWSqlDB db(std::move(services));
    db.loadDB(file);
    TxPoW txpow;
    txpow.setTimeMilli();
    txpow.setSuperParent(0,MiniData::fromHex("0xFFEEFF"));
    txpow.calculateTXPOWID();
    std::cout<<"IN :"<<txpow.toJSON().toString()<<std::endl;
    [[maybe_unused]] const auto id=txpow.getTxPoWID();
    db.addTxPoW(txpow,true);
    db.saveDB(false);
}

}
