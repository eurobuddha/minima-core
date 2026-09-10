#include "MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaUnicode.hpp"
#include <bit>
#include <iostream>
namespace minima {
namespace {
std::optional<std::u16string> addressKey(const std::optional<std::string>& value){return value?std::optional(unicode::decodeUTF8(*value)):std::nullopt;}
}
void MinimaDB::createDB(MinimaDBServices services){
    auto db=std::make_shared<MinimaDB>(std::move(services));
    {std::lock_guard lock(mInstanceLock);mMinimaDB=std::move(db);}
    bindRuntime();
}
std::shared_ptr<MinimaDB> MinimaDB::getDB(){std::lock_guard lock(mInstanceLock);return mMinimaDB;}
MinimaDB::MinimaDB(MinimaDBServices services):mServices(std::move(services)){
    if(!mServices.sql.connect||!mServices.sql.startupDebugLogs||!mServices.isShuttingDown||!mServices.postMainMessage||!mServices.postMainTimer||!mServices.postNotifyEvent||!mServices.setStartupError||!mServices.halt||!mServices.currentThreadName)
        throw std::invalid_argument("MinimaDB requires native SQL and Main runtime services");
    mArchive=newArchive();mTxPoWDB=newTxPoWDB();mTxPoWTree=std::make_shared<TxPowTree>();mCascade=std::make_shared<Cascade>();
    mUserDB=std::make_shared<UserDB>();mWallet=std::make_shared<Wallet>(mServices.sql);mTxBlockDB=std::make_shared<TxBlockDB>();
    mP2PDB=std::make_shared<p2p::P2PDB>();mP2P2DB=std::make_shared<p2p2::P2P2DB>();mMegaMMR=std::make_shared<MegaMMR>();
}
std::shared_ptr<ArchiveManager> MinimaDB::newArchive(){return std::make_shared<ArchiveManager>(mServices.sql,mServices.isShuttingDown);}
std::shared_ptr<TxPoWDB> MinimaDB::newTxPoWDB(){return std::make_shared<TxPoWDB>(TxPoWDBServices{mServices.sql,mServices.postMainMessage,[]{
    auto wallet=sqlRequired(getDB()).getWallet();ChainServices result;
    result.isAddressRelevant=[wallet](const auto& value){return sqlRequired(wallet).isAddressRelevant(value);};
    result.isAddressSimple=[wallet](const auto& value){return sqlRequired(wallet).isAddressSimple(value);};
    result.isKeyRelevant=[wallet](const auto& value){return sqlRequired(wallet).isKeyRelevant(value);};return result;
}});}
void MinimaDB::log(const std::string& text) const {if(mServices.sql.log)mServices.sql.log(text,true);else std::clog<<text<<'\n';}
void MinimaDB::readLock(bool lock){
    if(GeneralParams::DB_IGNORE_LOCKS)return;
    if(lock)mRWLock.readLock();else try{mRWLock.readUnlock();}catch(const std::exception& error){log(sqlExceptionText(error));}
}
void MinimaDB::writeLock(bool lock){
    if(GeneralParams::DB_IGNORE_LOCKS)return;
    if(lock){mRWLock.writeLock();auto name=mServices.currentThreadName();std::lock_guard guard(mDiagnosticLock);mCurrentWriteLockThread=std::move(name);}
    else mRWLock.writeUnlock();mCurrentWriteLockState=lock;
}
std::string MinimaDB::currentWriteLockThread(){std::lock_guard lock(mDiagnosticLock);return mCurrentWriteLockThread;}
void MinimaDB::safeReleaseReadWriteLock(){
    if(GeneralParams::DB_IGNORE_LOCKS)return;
    try{if(mRWLock.getWriteHoldCount()>0)mRWLock.writeUnlock();}catch(const std::exception&){}
    try{if(mRWLock.getReadHoldCount()>0)mRWLock.readUnlock();}catch(const std::exception&){}
}
void MinimaDB::resetCascadeAndTxPoWTree(){replace(mCascade,std::make_shared<Cascade>());replace(mTxPoWTree,std::make_shared<TxPowTree>());}
std::filesystem::path MinimaDB::getBaseDBFolder(){return javaUnixChildFile(GeneralParams::DATA_FOLDER,"databases");}
std::filesystem::path MinimaDB::getDBFile(const std::optional<std::string>& name){
    auto base=getBaseDBFolder();if(!name)throw JavaNullPointerException("");return javaUnixChildFile(base,*name);
}
int64_t MinimaDB::getDBFileSie(const std::string& name){return javaUnixFileLength(javaUnixChildFile(getBaseDBFolder(),name));}
void MinimaDB::loadWallet(const std::filesystem::path& base,bool announceEncryption){
    auto file=base/"walletsql"/"wallet";
    if(!GeneralParams::IS_MAIN_DBPASSWORD_SET)sqlRequired(owned(mWallet)).loadDB(file);
    else{if(announceEncryption)log("Using Encrypted SQL DB");sqlRequired(owned(mWallet)).loadEncryptedSQLDB(file,GeneralParams::MAIN_DBPASSWORD);}
}
void MinimaDB::loadAllDB(){
    writeLock(true);
    try{
        auto base=getBaseDBFolder();
        if(mServices.sql.startupDebugLogs())log("MinimaDB load Wallet..");loadWallet(base,true);
        if(mServices.sql.startupDebugLogs())log("MinimaDB load ArchiveDB..");auto archive=base/"archivesql";
        try{sqlRequired(owned(mArchive)).loadDB(archive/"archive");}
        catch(const std::exception& error){
            log(sqlExceptionText(error));log("ERROR loading ArchiveDB.. WIPE and RESYNC.. ");sqlRequired(owned(mArchive)).hardCloseDB();
            MiniFile::deleteFileOrFolder(javaUnixAbsoluteFile(archive),archive);replace(mArchive,newArchive());sqlRequired(owned(mArchive)).loadDB(archive/"archive");
        }
        if(mServices.sql.startupDebugLogs())log("MinimaDB load TxPoWDB..");auto txpow=base/"txpowsql";
        try{sqlRequired(owned(mTxPoWDB)).loadSQLDB(txpow/"txpow");}
        catch(const std::exception& error){
            log(sqlExceptionText(error));log("ERROR loading TxPoWSQLDB.. WIPE and RESYNC.. ");sqlRequired(owned(mTxPoWDB)).hardCloseSQLDB();
            MiniFile::deleteFileOrFolder(javaUnixAbsoluteFile(txpow),txpow);replace(mTxPoWDB,newTxPoWDB());sqlRequired(owned(mTxPoWDB)).loadSQLDB(txpow/"txpow");
        }
        if(mServices.sql.startupDebugLogs())log("MinimaDB load MaximaDB..");sqlRequired(owned(mUserDB)).loadDB(base/"userprefs.db");
        auto txn=std::make_shared<TxnDB>();txn->bindUserDBResolver([]{return sqlRequired(getDB()).getUserDB();});replace(mTxnDB,txn);txn->loadDB();
        if(GeneralParams::USE_SQL_COINDB)log("Using low ram SQL CoinDB in TxPoWTree");
        auto coins=base/"coindb";MiniFile::deleteFileOrFolder(javaUnixAbsoluteFile(coins),coins);CoinDB::createCoinDB(coins/"coins.db",mServices.sql);
        sqlRequired(owned(mCascade)).loadDB(base/"cascade.db");auto tree=base/"chaintree.db";
        log("Loading TxPowTree size : "+MiniFormat::formatSize(javaUnixFileLength(tree)));sqlRequired(owned(mTxPoWTree)).loadDB(tree);
        sqlRequired(owned(mP2PDB)).loadDB(base/"p2p.db");sqlRequired(owned(mP2P2DB)).loadDB(base/"p2p2.db");
        if(!Cascade::checkCascadeCorrect(sqlRequired(owned(mCascade)),[this](const auto& text){log(text);}))throw JavaCheckedException("Your Cascade is BROKEN.. please 'reset' your node.");
        if(GeneralParams::IS_MEGAMMR)sqlRequired(owned(mMegaMMR)).loadMMR(base/"megammr.mmr");
        else MiniFile::deleteFileOrFolder(javaUnixAbsoluteFile(base),base/"megammr.mmr");
        if(sqlRequired(owned(mCascade)).getTip()&&sqlRequired(owned(mTxPoWTree)).getRoot()){
            auto cascadeStart=sqlRequired(sqlRequired(owned(mCascade)).getTip()).getTxPoW().getBlockNumber();
            auto treeRoot=sqlRequired(sqlRequired(owned(mTxPoWTree)).getRoot()).getTxPoW().getBlockNumber();
            if(!treeRoot.isEqual(cascadeStart.increment()))throw JavaCheckedException("Your Cascade is BROKEN.. please 'reset' your node.");
            if(GeneralParams::IS_MEGAMMR&&!sqlRequired(owned(mMegaMMR)).isEmpty()){
                auto time=sqlRequired(owned(mMegaMMR)).getMMR().getBlockTime();
                if(!time.isEqual(treeRoot.decrement()))throw JavaCheckedException("Your MEGAMMR is BROKEN (does not start "+time.toString()+" where tree ends "+treeRoot.toString()+").. please 'reset' your node.");
            }
        }
        sqlRequired(getArchive()).checkCascadeRequired(getCascade().get());
    }catch(const std::exception& error){
        log("SERIOUS ERROR loadAllDB : ");log(sqlExceptionText(error));auto text=sqlExceptionText(error);
        if(!GeneralParams::RESCUE_MEGAMMR_NODE.empty()){
            log("RESCUE NODE FOUND.. attempting rescue @ "+GeneralParams::RESCUE_MEGAMMR_NODE);
            mServices.postMainTimer(std::make_shared<TimerMessage>(1000,"MAIN_DO_RESCUE"));
        }else if(GeneralParams::IS_MOBILE||GeneralParams::IS_JNLP)mServices.setStartupError(true,text);
        else {mServices.halt(0);throw std::logic_error("Runtime halt callback returned");}
    }
    writeLock(false);
}
void MinimaDB::loadArchiveAndTxPoWDB(bool resetWallet){
    writeLock(true);try{
        auto base=getBaseDBFolder();if(resetWallet){replace(mWallet,std::make_shared<Wallet>(mServices.sql));loadWallet(base,false);}
        replace(mArchive,newArchive());sqlRequired(owned(mArchive)).loadDB(base/"archivesql"/"archive");
        replace(mTxPoWDB,newTxPoWDB());sqlRequired(owned(mTxPoWDB)).loadSQLDB(base/"txpowsql"/"txpow");
    }catch(const std::exception& error){log("SERIOUS ERROR loadArchiveAndTxPoWDB");log(sqlExceptionText(error));}writeLock(false);
}
void MinimaDB::loadDBsForRestoreSync(){
    writeLock(true);try{
        auto base=getBaseDBFolder();replace(mWallet,std::make_shared<Wallet>(mServices.sql));loadWallet(base,false);
        replace(mArchive,newArchive());sqlRequired(owned(mArchive)).loadDB(base/"archivesql"/"archive");
        replace(mTxPoWDB,newTxPoWDB());sqlRequired(owned(mTxPoWDB)).loadSQLDB(base/"txpowsql"/"txpow");
        replace(mCascade,std::make_shared<Cascade>());sqlRequired(owned(mCascade)).loadDB(base/"cascade.db");
        replace(mTxPoWTree,std::make_shared<TxPowTree>());sqlRequired(owned(mTxPoWTree)).loadDB(base/"chaintree.db");
    }catch(const std::exception& error){log("SERIOUS ERROR loadDBsForRestoreSync");log(sqlExceptionText(error));}writeLock(false);
}
void MinimaDB::refreshSQLDB(){sqlRequired(owned(mArchive)).closeAndReopen();sqlRequired(sqlRequired(owned(mTxPoWDB)).getSQLDB()).closeAndReopen();}
void MinimaDB::saveAllDB(bool compact){log("Saving State..");saveState();log("Saving SQL..");saveSQL(compact);log("All saved..");}
void MinimaDB::saveSQL(bool compact){
    writeLock(true);try{
        try{
            log("Wallet shutdown..");sqlRequired(owned(mWallet)).saveDB(true);log("TxPowDB shutdown..");sqlRequired(owned(mTxPoWDB)).saveDB(compact);
            log("ArchiveDB shutdown..");sqlRequired(owned(mArchive)).saveDB(compact);log("CoinDB shutdown..");sqlRequired(CoinDB::getTxPoWTreeCoinDB()).saveDB(false);log("All SQL DB Shutdown..");
        }catch(const std::exception& error){log(sqlExceptionText(error));}
    }catch(...){writeLock(false);throw;}writeLock(false);
}
void MinimaDB::fullDBRestartMemFree(){
    writeLock(true);try{
        auto base=getBaseDBFolder();sqlRequired(owned(mTxPoWDB)).wipeDBRAM();sqlRequired(sqlRequired(owned(mTxPoWDB)).getSQLDB()).cleanDB(true);
        sqlRequired(sqlRequired(owned(mTxPoWDB)).getOnChainDB()).cleanDB(true);sqlRequired(owned(mArchive)).checkForCleanDB();
        sqlRequired(owned(mTxPoWDB)).saveDB(false);sqlRequired(owned(mArchive)).saveDB(false);sqlRequired(owned(mWallet)).saveDB(false);
        replace(mWallet,std::make_shared<Wallet>(mServices.sql));loadWallet(base,false);
        replace(mArchive,newArchive());sqlRequired(owned(mArchive)).loadDB(base/"archivesql"/"archive");
        replace(mTxPoWDB,newTxPoWDB());sqlRequired(owned(mTxPoWDB)).loadSQLDB(base/"txpowsql"/"txpow");
    }catch(const SqlException& error){std::cerr<<sqlExceptionText(error)<<'\n';}writeLock(false);
}
void MinimaDB::saveState(){
    if(!mAllowSaveState)return;readLock(true);try{
        auto base=getBaseDBFolder();sqlRequired(owned(mTxnDB)).saveDB();sqlRequired(owned(mUserDB)).saveDB(base/"userprefs.db");
        sqlRequired(owned(mP2PDB)).saveDB(base/"p2p.db");sqlRequired(owned(mP2P2DB)).saveDB(base/"p2p2.db");
        sqlRequired(owned(mCascade)).saveDB(base/"cascade.db");sqlRequired(owned(mTxPoWTree)).saveDB(base/"chaintree.db");
        if(GeneralParams::IS_MEGAMMR)sqlRequired(owned(mMegaMMR)).saveMMR(base/"megammr.mmr");
    }catch(const std::exception& error){log(sqlExceptionText(error));}readLock(false);
}
void MinimaDB::saveUserDB(){
    if(!mAllowSaveState)return;readLock(true);try{auto base=getBaseDBFolder();sqlRequired(owned(mUserDB)).saveDB(base/"userprefs.db");}
    catch(const std::exception& error){log(sqlExceptionText(error));}readLock(false);
}
void MinimaDB::saveP2PDB(){
    if(!mAllowSaveState)return;try{auto base=getBaseDBFolder();sqlRequired(owned(mP2PDB)).saveDB(base/"p2p.db");sqlRequired(owned(mP2P2DB)).saveDB(base/"p2p2.db");}
    catch(const std::exception& error){log(sqlExceptionText(error));}
}
void MinimaDB::addCoinNotify(const std::optional<std::string>& address){std::lock_guard lock(mNotifyLock);mCoinNotify.insert(addressKey(address));}
bool MinimaDB::removeCoinNotify(const std::optional<std::string>& address){std::lock_guard lock(mNotifyLock);return mCoinNotify.erase(addressKey(address))!=0;}
bool MinimaDB::checkCoinNotify(const std::optional<std::string>& address){std::lock_guard lock(mNotifyLock);return mCoinNotify.contains(addressKey(address));}
ChainServices MinimaDB::chainServices(){
    ChainServices service;
    service.isAddressRelevant=[](const auto& value){return sqlRequired(sqlRequired(getDB()).getWallet()).isAddressRelevant(value);};
    service.isAddressSimple=[](const auto& value){return sqlRequired(sqlRequired(getDB()).getWallet()).isAddressSimple(value);};
    service.isKeyRelevant=[](const auto& value){return sqlRequired(sqlRequired(getDB()).getWallet()).isKeyRelevant(value);};
    service.checkCoinNotify=[](const auto& value){return sqlRequired(getDB()).checkCoinNotify(value);};
    service.postNotifyEvent=[](const auto& type,const auto& data){sqlRequired(getDB()).mServices.postNotifyEvent(type,data);};
    service.postMessage=[](const auto& type){sqlRequired(getDB()).mServices.postMainMessage(std::make_shared<Message>(type));};
    service.insertCoin=[](const auto& id,const auto& coin,const auto& block){sqlRequired(CoinDB::getTxPoWTreeCoinDB()).insertCoin(id,coin,block);};
    service.getAllCoins=[](const auto& id){auto coins=sqlRequired(CoinDB::getTxPoWTreeCoinDB()).getAllCoins(id);std::vector<Coin> values;for(const auto& coin:coins)values.push_back(coin?*coin:Coin(nullptr));return values;};
    service.getAllRelevantUnspentCoins=[](auto node){return TxPoWSearcher::getAllRelevantUnspentCoins(std::move(node));};
    service.addOnChainTxPoW=[](const auto& id,const auto& block,const auto& txpow){sqlRequired(sqlRequired(sqlRequired(getDB()).getTxPoWDB()).getOnChainDB()).addOnChainTxPoW(id,&block,txpow);};
    service.addSQLTxPoW=[](const auto& txpow){sqlRequired(sqlRequired(getDB()).getTxPoWDB()).addSQLTxPoW(std::make_shared<TxPoW>(txpow));};
    service.addTxBlock=[](const auto& block){sqlRequired(sqlRequired(getDB()).getTxBlockDB()).addTxBlock(std::make_shared<TxBlock>(block));};return service;
}
void MinimaDB::bindRuntime(){
    TxPoWGenerator::services=consensusServices();TxPoWChecker::services=consensusServices();
    MMR::megaMMRLookup=[]{return sqlRequired(sqlRequired(getDB()).getMegaMMR()).getSharedMMR();};
    TxPoWTreeNode::services=chainServices();TxnDB::runtimeLookup=[]{return sqlRequired(getDB()).getCustomTxnDB();};
    TxPoWSearcher::services={[]{return sqlRequired(sqlRequired(getDB()).getTxPoWTree()).getTip();},
        []{return sqlRequired(getDB()).getMegaMMR();},
        [](bool lock){sqlRequired(getDB()).readLock(lock);},[](const auto& id){return sqlRequired(sqlRequired(getDB()).getTxPoWDB()).getTxPoW(id);}};
}
IBDServices MinimaDB::ibdServices(){
    return {[]{return sqlRequired(getDB()).getTxPoWTree();},[]{return sqlRequired(getDB()).getCascade();},[]{
        auto archive=sqlRequired(getDB()).getArchive();auto result=std::make_shared<IBDArchive>();
        result->exists=[archive](const auto& id){return sqlRequired(archive).exists(id);};
        result->loadBlockRange=[archive](const auto& a,const auto& b,bool descending){return sqlRequired(archive).loadBlockRange(a,b,descending);};
        result->loadSyncBlockRange=[archive](const auto& start){return sqlRequired(archive).loadSyncBlockRange(start);};
        result->loadLastBlock=[archive]{return sqlRequired(archive).loadLastBlock();};result->loadCascade=[archive]{return sqlRequired(archive).loadCascade();};return result;
    },[](bool lock){sqlRequired(getDB()).readLock(lock);},[]{return sqlRequired(getDB()).mServices.isShuttingDown();}};
}
ConsensusServices MinimaDB::consensusServices(){
    return {[]{return sqlRequired(sqlRequired(getDB()).getTxPoWTree()).getTip();},[]{return sqlRequired(sqlRequired(getDB()).getTxPoWTree()).getRoot();},
        []{return sqlRequired(sqlRequired(getDB()).getCascade()).getTip();},[](const auto& id){return sqlRequired(sqlRequired(getDB()).getTxPoWDB()).checkMempoolCoins(id);},
        []{return sqlRequired(sqlRequired(getDB()).getTxPoWDB()).getAllUnusedTxns();},[](const auto& id){sqlRequired(sqlRequired(getDB()).getTxPoWDB()).removeMemPoolTxPoW(id);},
        []{return sqlRequired(sqlRequired(getDB()).getUserDB()).getMagicDesiredKISSVM();},[]{return sqlRequired(sqlRequired(getDB()).getUserDB()).getMagicMaxTxPoWSize();},
        []{return sqlRequired(sqlRequired(getDB()).getUserDB()).getMagicMaxTxns();},[]{return sqlRequired(sqlRequired(getDB()).getUserDB()).getHashRate();}};
}
}
