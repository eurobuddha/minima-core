#include "TxPoWDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "support/JavaUnixFile.hpp"
#include <iostream>
namespace minima {
namespace {
const std::string& ramID(const std::optional<std::string>& id){
    if(!id)throw SqlRuntimeException("java.lang.NullPointerException","null ConcurrentHashMap key");
    return *id;
}
}
TxPoWDB::TxPoWDB(TxPoWDBServices services,std::function<int64_t()> sqlClock)
    :mRamDB(std::make_shared<RamDB>()),
     mSqlDB(std::make_shared<TxPoWSqlDB>(services.sql,sqlClock)),
     mOnChainDB(std::make_shared<TxPoWOnChainDB>(services.sql,std::move(sqlClock))),
     mServices(std::move(services)) {
    if(!mServices.postMainMessage||!mServices.getWallet)
        throw std::invalid_argument("TxPoWDB requires Main and wallet services");
}
void TxPoWDB::log(const std::string& text){if(mServices.sql.log)mServices.sql.log(text,true);else std::clog<<text<<'\n';}
void TxPoWDB::loadSQLDB(const SqlDB::File& file){
    auto normalized=javaUnixFile(file);sqlRequired(mSqlDB).loadDB(normalized);
    if(!normalized)throw SqlRuntimeException("java.lang.NullPointerException","null SQL file");
    auto parent=*normalized==normalized->root_path()?std::filesystem::path():normalized->parent_path();
    sqlRequired(mOnChainDB).loadDB(parent/"onchain");
}
void TxPoWDB::hardCloseSQLDB(){sqlRequired(mSqlDB).hardCloseDB();sqlRequired(mOnChainDB).hardCloseDB();}
void TxPoWDB::saveDB(bool compact){sqlRequired(mSqlDB).saveDB(compact);sqlRequired(mOnChainDB).saveDB(compact);}
bool TxPoWDB::addTxPoW(std::shared_ptr<TxPoW> txpow){
    auto id=sqlRequired(txpow).getTxPoWID();
    try{
        auto message=std::make_shared<Message>("MAIN_AUTOBACKUP_TXPOW");
        message->addObject("txpow",MessageObject(txpow));mServices.postMainMessage(std::move(message));
    }catch(const std::exception& error){log("STORESQl TxPoW : "+sqlExceptionText(error));}
    if(!sqlRequired(mRamDB).exists(id))mRamDB->addTxPoW(txpow);
    if(!sqlRequired(mSqlDB).exists(id)){
        auto wallet=mServices.getWallet();bool relevant=TxPoWSearcher::checkTxPoWRelevant(*txpow,wallet);
        mSqlDB->addTxPoW(txpow.get(),relevant);return relevant;
    }
    auto wallet=mServices.getWallet();return TxPoWSearcher::checkTxPoWRelevant(*txpow,wallet);
}
void TxPoWDB::addSQLTxPoW(std::shared_ptr<TxPoW> txpow){
    auto id=sqlRequired(txpow).getTxPoWID();
    if(!sqlRequired(mSqlDB).exists(id)){
        auto wallet=mServices.getWallet();bool relevant=TxPoWSearcher::checkTxPoWRelevant(*txpow,wallet);
        mSqlDB->addTxPoW(txpow.get(),relevant);
    }
}
std::shared_ptr<TxPoW> TxPoWDB::getTxPoW(const std::optional<std::string>& id){
    auto txpow=sqlRequired(mRamDB).getTxPoW(ramID(id));
    if(!txpow)txpow=sqlRequired(mSqlDB).getTxPoW(id);return txpow;
}
TxPoWDB::TxPoWs TxPoWDB::getAllTxPoW(const IDs* ids){
    TxPoWs result;if(!ids)throw SqlRuntimeException("java.lang.NullPointerException","null TxPoW ID list");
    for(const auto& id:*ids){auto txpow=getTxPoW(id);if(txpow)result.push_back(std::move(txpow));}return result;
}
bool TxPoWDB::exists(const std::optional<std::string>& id){
    bool found=sqlRequired(mRamDB).exists(ramID(id));if(!found)found=sqlRequired(mSqlDB).exists(id);return found;
}
TxPoWDB::TxPoWs TxPoWDB::getChildBlocks(const std::optional<std::string>& parent){auto ids=sqlRequired(mSqlDB).getChildBlocks(parent);return getAllTxPoW(ids);}
int32_t TxPoWDB::getRamSize(){return sqlRequired(mRamDB).getSize();}
int32_t TxPoWDB::getSqlSize(){return sqlRequired(mSqlDB).getSize();}
SqlDB::File TxPoWDB::getSqlFile(){return sqlRequired(mSqlDB).getSQLFile();}
void TxPoWDB::cleanDBRAM(){sqlRequired(mRamDB).cleanDB();}
void TxPoWDB::wipeDBRAM(){sqlRequired(mRamDB).wipeRamDB();}
void TxPoWDB::cleanDBSQL(){sqlRequired(mSqlDB).cleanDB();sqlRequired(mOnChainDB).cleanDB();}
void TxPoWDB::clearMainChainTxns(){sqlRequired(mRamDB).clearMainChainTxns();}
void TxPoWDB::setOnMainChain(const std::optional<std::string>& id){sqlRequired(mRamDB).setOnMainChain(ramID(id));}
void TxPoWDB::setInCascade(const std::optional<std::string>& id){sqlRequired(mRamDB).setInCascade(ramID(id));}
TxPoWDB::TxPoWs TxPoWDB::getAllUnusedTxns(){return sqlRequired(mRamDB).getAllUnusedTxns();}
void TxPoWDB::removeMemPoolTxPoW(const std::optional<std::string>& id){sqlRequired(mRamDB).remove(ramID(id));}
bool TxPoWDB::checkMempoolCoins(const MiniData& id){return sqlRequired(mRamDB).checkForCoinID(id);}
std::shared_ptr<RamDB::Map> TxPoWDB::getCompleteMemPool(){return sqlRequired(mRamDB).getCompleteMemPool();}
}
