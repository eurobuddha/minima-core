#include "Main.hpp"
#include "commands/backup/mmrsync/megammrsync.hpp"
#include "org/minima/system/commands/network/rpc.hpp"
#include "commands/CommandRunner.hpp"
#include "commands/network/connect.hpp"
#include "commands/network/webhooks.hpp"
#include "commands/search/keys.hpp"
#include "commands/send/sendpoll.hpp"
#include "commands/txn/txnutils.hpp"
#include "commands/txn/txnminepost.hpp"
#include "genesis/GenesisMMR.hpp"
#include "genesis/GenesisTxPoW.hpp"
#include "network/minima/NIOMessage.hpp"
#include "params/GeneralParams.hpp"
#include "params/GlobalParams.hpp"
#include "org/minima/objects/Pulse.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/NativeThread.hpp"
#include "support/MySQLConfiguration.hpp"
#include "support/CommandResult.hpp"
#include "support/RawTxnCommands.hpp"
#include <bit>
#include <cstdlib>
namespace minima {
namespace {
void releaseLocks(){auto db=MinimaDB::getDB();if(db)db->safeReleaseReadWriteLock();}
void log(const std::string& text){MinimaLogger::log(text);}
json::JSONValue jsonObject(const std::shared_ptr<json::JSONObject>& object){json::JSONValue value;if(object)value.value=object;return value;}
void logError(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
int64_t subtract(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
std::shared_ptr<Main> mainOwner(){auto owner=Main::getInstance();sqlRequired(owner);return owner;}
std::shared_ptr<TxPowTree> tree(){return sqlRequired(MinimaDB::getDB()).getTxPoWTree();}
std::shared_ptr<UserDB> user(){return sqlRequired(MinimaDB::getDB()).getUserDB();}
std::shared_ptr<Wallet> wallet(){return sqlRequired(MinimaDB::getDB()).getWallet();}
std::shared_ptr<TxPoW> powObject(const Message& message){auto value=message.getObject("txpow");if(value.isNull())return nullptr;try{return value.as<std::shared_ptr<TxPoW>>();}catch(const std::bad_any_cast&){throw JavaClassCastException("message object is not a TxPoW");}}
void removeFile(const SqlDB::File& file){if(!file)throw JavaNullPointerException("null database file");std::error_code error;std::filesystem::remove(*file,error);}
}
Main::Main(MainServices services):MessageProcessor("MAIN",releaseLocks),mServices(std::move(services)),mUptimeMilli(mServices.now()){
    if(!mServices.sleep)mServices.sleep=[](int64_t delay){if(delay<0)throw JavaIllegalArgumentException("timeout value is negative");std::this_thread::sleep_for(std::chrono::milliseconds(delay));};
    if(!mServices.halt)mServices.halt=[](int code){std::_Exit(code);};
    if(!mServices.exit)mServices.exit=[](int code){std::exit(code);};
    if(!mServices.currentThreadName)mServices.currentThreadName=currentNativeThreadName;
    if(!mServices.getCurrentMegaMMRIBD)mServices.getCurrentMegaMMRIBD=commands::megammrsync::getCurrentMegaMMRIBD;
}
std::shared_ptr<Main> Main::getInstance(){std::lock_guard lock(mStaticLock);return mMainInstance;}
void Main::ClearMainInstance(){std::shared_ptr<Main> retired;{std::lock_guard lock(mStaticLock);retired=std::move(mMainInstance);}if(retired)log("Main Instance Cleared..");}
std::shared_ptr<MessageListener> Main::getMinimaListener(){std::lock_guard lock(mStaticLock);return MINIMA_LISTENER;}
void Main::setMinimaListener(std::shared_ptr<MessageListener> listener){{std::lock_guard lock(mStaticLock);MINIMA_LISTENER.swap(listener);}}
std::shared_ptr<Main> Main::create(MainServices services){
    auto owner=std::shared_ptr<Main>(new Main(std::move(services)));
    if(STARTUP_DEBUG_LOGS)log("MAIN init.. start");owner->mUptimeMilli=owner->mServices.now();
    std::shared_ptr<Main> retired;{std::lock_guard lock(mStaticLock);retired=std::move(mMainInstance);mMainInstance=owner;}retired.reset();
    try{owner->bindRuntime();owner->startMessageProcessor();owner->initialize();}catch(...){owner->cleanNativeOwners();{std::lock_guard lock(mStaticLock);if(mMainInstance==owner)mMainInstance.reset();}throw;}return owner;
}
Main::~Main(){cleanNativeOwners();}
void Main::cleanNativeOwners() noexcept{
    try{if(mTimerOwner){mTimerOwner->stop();mTimerOwner->join();}}catch(...){}
    try{if(auto net=getNetworkManager()){net->hardShutDown();net->join();}}catch(...){}
    for(const auto& worker:std::vector<std::shared_ptr<MessageProcessor>>{getNotifyManager(),getTxPoWProcessor(),getTxPoWMiner(),getSendPoll()})if(worker)try{worker->stopMessageProcessor();worker->joinMessageProcessor();}catch(...){}
    try{MessageProcessor::stopMessageProcessor();joinMessageProcessor();}catch(...){}
}
MinimaDBServices Main::databaseServices(){
    auto sql=mServices.sql;sql.startupDebugLogs=[]{return STARTUP_DEBUG_LOGS;};
    return {std::move(sql),[]{return mainOwner()->isShuttingDown();},[](auto msg){mainOwner()->PostMessage(msg);},[](auto timer){mainOwner()->PostTimerMessage(timer);},[](const auto& type,const auto& data){mainOwner()->PostNotifyEvent(type,data);},[](bool error,const auto& message){mainOwner()->setStartUpError(error,message);},mServices.halt,mServices.currentThreadName};
}
std::shared_ptr<TxPoWProcessor> Main::newProcessor(){
    TxPoWProcessorServices s;s.releaseLocks=releaseLocks;s.isShuttingDown=[]{return mainOwner()->isShuttingDown();};s.isShuttingDownOrRestoring=[]{return mainOwner()->isShuttongDownOrRestoring();};s.postMain=[](auto msg){mainOwner()->PostMessage(msg);};s.postNotifyEvent=[](const auto& type,const auto& data){mainOwner()->PostNotifyEvent(type,data);};s.setSyncIBD=[](bool sync){mainOwner()->setSyncIBD(sync);};s.log=log;s.now=mServices.now;return TxPoWProcessor::create(std::move(s));
}
std::shared_ptr<NetworkManager> Main::newNetwork(){
    NetworkManagerServices s;s.currentTimeMillis=mServices.now;s.getUserDB=user;
    s.nio.releaseLocks=releaseLocks;s.nio.isShuttingDown=[]{return mainOwner()->isShuttingDown();};s.nio.isRestoring=[]{return mainOwner()->isRestoring();};
    s.nio.txPoWExists=[](const auto& id){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).exists(id);};
    s.nio.clearClientSync=[](const auto& uid){NIOMessage::mlastSyncReq.remove(uid);NIOMessage::mLastChainSync.remove(uid);};
    s.nio.getRoot=[]{return sqlRequired(tree()).getRoot();};s.nio.loadLastArchiveBlock=[]{return sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).loadLastBlock();};s.nio.processIncoming=NIOMessage::dispatch;s.nio.currentTimeMillis=mServices.now;
    s.p2p={releaseLocks,[]{sqlRequired(MinimaDB::getDB()).saveP2PDB();},[]{return sqlRequired(MinimaDB::getDB()).getP2PDB();},[]{return sqlRequired(tree()).getTip();},mServices.now};
    s.p2p2={releaseLocks,[]{return sqlRequired(MinimaDB::getDB()).getP2P2DB();},[]{return sqlRequired(MinimaDB::getDB()).getP2PDB();}};return NetworkManager::create(std::move(s));
}
void Main::bindRuntime(){
    MySQLConnect::runtimeServices=mServices.mysql;
    NIOManager::runtimeLookup=[]{return mainOwner()->getNIOManager();};NIOClient::runtimeLookup=[]{return sqlRequired(mainOwner()->getNIOManager()).clientRuntime();};
    IBD::services=MinimaDB::ibdServices();
    auto peers=[]{return sqlRequired(std::dynamic_pointer_cast<p2p::P2PManager>(sqlRequired(mainOwner()->getNetworkManager()).getP2PManager())).getPeersCopy();};
    Greeting::services={[](bool lock){sqlRequired(MinimaDB::getDB()).readLock(lock);},[]{return std::optional<std::string>(sqlRequired(user()).getWelcome());},[]{return sqlRequired(tree()).getTip();},peers};
    auto& n=NIOMessage::services;n.isShuttingDownOrRestoring=[]{return mainOwner()->isShuttongDownOrRestoring();};n.isSyncIBD=[]{return mainOwner()->isSyncIBD();};n.getTree=tree;n.getCascade=[]{return sqlRequired(MinimaDB::getDB()).getCascade();};
    n.txPoWExists=[](const auto& id){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).exists(id);};n.getTxPoW=[](const auto& id){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getTxPoW(id);};
    n.findTxBlock=[](const auto& id){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxBlockDB()).findTxBlock(id);};n.loadArchiveBlock=[](const auto& id){return sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).loadBlock(id);};
    n.addInvalidPeer=[](const auto& peer){p2p::P2PFunctions::addInvalidPeer(peer);};n.postP2P=[](auto msg){sqlRequired(sqlRequired(mainOwner()->getNetworkManager()).getP2PManager()).PostMessage(msg);};
    n.postP2PTimer=[](auto timer){sqlRequired(sqlRequired(mainOwner()->getNetworkManager()).getP2PManager()).PostTimerMessage(timer);};n.postMainTimer=[](auto timer){mainOwner()->PostTimerMessage(timer);};n.notifyEvent=[](const auto& event,const auto& data){mainOwner()->PostNotifyEvent(event,data);};
    n.postProcessIBD=[](auto ibd,auto uid){sqlRequired(mainOwner()->getTxPoWProcessor()).postProcessIBD(ibd,uid);};n.postProcessSyncIBD=[](auto ibd,auto uid){sqlRequired(mainOwner()->getTxPoWProcessor()).postProcessSyncIBD(ibd,uid);};n.postProcessTxPoW=[](auto pow){sqlRequired(mainOwner()->getTxPoWProcessor()).postProcessTxPoW(pow);};n.postProcessTxBlock=[](auto block){sqlRequired(mainOwner()->getTxPoWProcessor()).postProcessTxBlock(block);};n.mineTxPoWAsync=[](auto pow){sqlRequired(mainOwner()->getTxPoWMiner()).mineTxPoWAsync(pow);};
    n.getPeersCopy=peers;n.getP2PClients=[]{return sqlRequired(std::dynamic_pointer_cast<p2p::P2PManager>(sqlRequired(mainOwner()->getNetworkManager()).getP2PManager())).getClients();};n.getP2P2Peers=[]{return sqlRequired(sqlRequired(MinimaDB::getDB()).getP2P2DB()).getAllKnownPeers();};n.getCurrentMegaMMRIBD=mServices.getCurrentMegaMMRIBD;n.getAutomineTimer=[]{return mainOwner()->AUTOMINE_TIMER.load();};n.currentTimeMillis=mServices.now;
    commands::rpc::sleep=mServices.sleep;commands::connect::postMessage=[](auto msg){sqlRequired(mainOwner()->getNIOManager()).PostMessage(msg);};commands::webhooks::runtimeLookup=[]{return mainOwner()->getNotifyManager();};commands::keys::postMessage=[](auto msg){mainOwner()->PostMessage(msg);};commands::txnminepost::postMessage=commands::keys::postMessage;commands::txnutils::getMiner=[]{return mainOwner()->getTxPoWMiner();};commands::sendpoll::getManager=[]{return mainOwner()->getSendPoll();};commands::vault::getMain=[]{return std::static_pointer_cast<commands::vault::MainReceiver>(getInstance());};
    auto logger=MinimaLogger::getRuntime();logger.collectMemory=mServices.collectMemory;logger.getMain=[]{return std::static_pointer_cast<MinimaLogger::MainReceiver>(getInstance());};MinimaLogger::setRuntime(std::move(logger));
}
void Main::initialize(){
    if(GeneralParams::LOAD_JNI){try{if(!mServices.loadNativeLibrary)throw JavaError("java.lang.UnsatisfiedLinkError","No native library loader configured");mServices.loadNativeLibrary();log("JNI library loaded");}catch(const JavaError& error){log("ERROR : Could not load native JNI lib (MUST add -Djava.library.path=..) "+nativeExceptionText(error));mServices.exit(1);}}
    TimerProcessor::createTimerProcessor();mTimerOwner=TimerProcessor::getTimerProcessor();
    if(GeneralParams::PRIVATE){std::error_code error;bool exists=std::filesystem::exists(std::filesystem::path(GeneralParams::DATA_FOLDER)/"databases"/"userprefs.db",error);if(exists&&!GeneralParams::CLEAN)log("SOLO NETWORK : userdb found : not first run.. no -genesis..");else{log("SOLO NETWORK : userdb not found : FIRST RUN.. creating genesis coins..");GeneralParams::CLEAN=true;GeneralParams::GENESIS=true;}}
    if(GeneralParams::CLEAN){log("Wiping previous config files..");MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,GeneralParams::DATA_FOLDER);}
    if(STARTUP_DEBUG_LOGS)log("MinimaDB create.. start");MinimaDB::createDB(databaseServices());if(STARTUP_DEBUG_LOGS)log("MinimaDB create.. finish");
    if(STARTUP_DEBUG_LOGS)log("Load all DB.. start");sqlRequired(MinimaDB::getDB()).loadAllDB();if(STARTUP_DEBUG_LOGS)log("Load all DB.. finish");
    if(!GeneralParams::MYSQL_DB_DETAILS.empty())mysql_detail::configure(GeneralParams::MYSQL_DB_DETAILS,mServices.sleep);
    if(sqlRequired(user()).isSlaveNode()){GeneralParams::CONNECT_LIST=sqlRequired(user()).getSlaveNodeHost();GeneralParams::P2P_ENABLED=false;GeneralParams::TXBLOCK_NODE=true;GeneralParams::NO_SYNC_IBD=true;GeneralParams::IS_ACCEPTING_IN_LINKS=false;log("Slave Mode ENABLED master:"+GeneralParams::CONNECT_LIST);}
    TxPoWMiner::calculateHashRateOld(MiniNumber(10000));MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,std::filesystem::path(GeneralParams::DATA_FOLDER)/"archiverestore");
    auto rate=TxPoWMiner::calculateHashSpeed(MiniNumber(GeneralParams::INIT_HASHTEST_AMOUNT));sqlRequired(user()).setHashRate(rate);log("Calculate device hash rate ("+std::to_string(GeneralParams::INIT_HASHTEST_AMOUNT)+" hashes) : "+rate.div(MiniNumber::MILLION).setSignificantDigits(4).toString()+" MHs");
    try{mInitKeysCreated=sqlRequired(wallet()).initDefaultKeys(3);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){log(nativeExceptionText(error));}
    replace(mNotifyManager,NotifyManager::create({releaseLocks,user,getMinimaListener,{},{}}));replace(mTxPoWProcessor,newProcessor());
    TxPoWMinerServices miner;miner.releaseLocks=releaseLocks;miner.postMain=[](auto msg){mainOwner()->PostMessage(msg);};miner.automineTimer=[]{return mainOwner()->AUTOMINE_TIMER.load();};miner.log=log;miner.now=mServices.now;replace(mTxPoWMiner,TxPoWMiner::create(std::move(miner)));
    if(GeneralParams::GENESIS)doGenesis();p2p::P2PFunctions::clearInvalidPeers();replace(mNetwork,newNetwork());
    replace(mSendPoll,SendPollManager::create([](const auto& text){if(!text)throw JavaNullPointerException("null command");return CommandRunner::getRunner().runSingleCommand(*text);},releaseLocks));
    AUTOMINE_TIMER=MiniNumber::THOUSAND.div(GlobalParams::MINIMA_BLOCK_SPEED).getAsLong();getTxPoWMiner()->PostTimerMessage(std::make_shared<TimerMessage>(AUTOMINE_TIMER,TxPoWMiner::TXPOWMINER_MINEPULSE));
    schedule(GeneralParams::USER_PULSE_FREQ,MAIN_PULSE);schedule(GeneralParams::GENESIS?10000:180000,MAIN_CLEANDB_RAM);schedule(600000,MAIN_CLEANDB_SQL);schedule(SYSTEMCLEAN_TIMER,MAIN_SYSTEMCLEAN);schedule(CHECKER_TIMER,MAIN_CHECKER);schedule(30000,MAIN_INIT_KEYS);schedule(NETRESET_TIMER,MAIN_NETRESET);schedule(300000,MAIN_AUTOBACKUP);schedule(MAIN_AUTOBACKUP_MYSQL_TIMER,MAIN_AUTOBACKUP_MYSQL);schedule(P2PNETMDS_TIMER,MAIN_P2PNETMDS_CHECKER);collectMemory();
    if(GeneralParams::TXBLOCK_NODE){if(GeneralParams::CONNECT_LIST.find(',')!=std::string::npos){log("[!] Can ONLY connect to 1 host in slave mode.. stopping");mServices.exit(1);}log("Running in slave mode. Will Connect to "+GeneralParams::CONNECT_LIST);}
    p2p::P2PFunctions::clearInvalidPeers();NIOMessage::mHaveSentIBDRecently.clear();STARTUP_COMPLETE=true;
}
json::JSONObject Main::runSingleMinimaCMD(const std::optional<std::string>& input){if(!input)throw JavaNullPointerException("null command input");return CommandRunner::getRunner().runSingleCommand(javaStringTrim(*input));}
void Main::setSyncIBD(bool sync){if(GeneralParams::IBDSYNC_LOGS)log(std::string("SYNC IBD LOCK : ")+(sync?"true":"false"));mSyncIBD=sync;}
void Main::setStartUpError(bool error,const std::optional<std::string>& message){STARTUP_ERROR=error;std::lock_guard lock(mErrorLock);STARTUP_ERROR_MSG=message;}
std::optional<std::string> Main::getStartupErrorMsg() const{std::lock_guard lock(mErrorLock);return STARTUP_ERROR_MSG;}
int64_t Main::getUptimeMilli() const{return subtract(mServices.now(),mUptimeMilli);}
std::shared_ptr<NIOManager> Main::getNIOManager() const{return sqlRequired(getNetworkManager()).getNIOManager();}
int32_t Main::getAllDefaultKeysSize(){return sqlRequired(wallet()).getDefaultKeysNumber();}
void Main::doGenesis(){
    auto row=sqlRequired(wallet()).createNewSimpleAddress(true);auto address=sqlRequired(row).getAddress();if(!address)throw JavaNullPointerException("null genesis address");
    std::shared_ptr<TxPoW> genesis=std::make_shared<GenesisTxPoW>(*address);sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).addTxPoW(genesis);
    auto block=std::make_shared<TxBlock>(std::make_shared<GenesisMMR>(),genesis,std::make_shared<TxBlock::TxPoWs>());auto node=std::make_shared<TxPoWTreeNode>(block);
    sqlRequired(tree()).setRoot(node);sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).setOnMainChain(genesis->getTxPoWID());
}
void Main::shutdownGenProcs(){
    TimerProcessor::stopTimerProcessor();sqlRequired(getNetworkManager()).shutdownNetwork();sqlRequired(getTxPoWMiner()).stopMessageProcessor();sqlRequired(getSendPoll()).stopMessageProcessor();
    int64_t waited=0;while(!sqlRequired(getNetworkManager()).isShutDownComplete()){mServices.sleep(250);waited+=250;if(waited>10000){log("Network shutdown took too long..");getNetworkManager()->hardShutDown();break;}}
}
void Main::shutdownFinalProcs(bool shutdownMDS){
    if(shutdownMDS&&!mHaveShutDownMDS.exchange(true))sqlRequired(getNotifyManager()).shutDown();
    log("Shutdown TxPoWProcessor..");sqlRequired(getTxPoWProcessor()).stopMessageProcessor();getTxPoWProcessor()->waitToShutDown();
}
void Main::shutdown(bool compact){
    if(mShuttingdown){log("Shutdown called when already shutting down..");return;}
    log(compact?"Shut down started.. Compacting All Databases":"Shut down started..");mShuttingdown=true;
    try{sqlRequired(wallet()).shuttingDown();shutdownGenProcs();shutdownFinalProcs();log("Saving all db");sqlRequired(MinimaDB::getDB()).saveAllDB(compact);stopMessageProcessor();log("Main thread shutdown");waitToShutDown();log("Shut down completed OK..");NotifyMainListenerOfShutDown();}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){log("ERROR Shutting down..");logError(error);}
}
void Main::NotifyMainListenerOfShutDown(){ClearMainInstance();if(mShutDownSentToListener.exchange(true))return;try{NotifyMainListenerOnly("SHUTDOWN");}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){logError(error);}}
void Main::restoreReady(bool shutdownMDS){mRestoring=true;shutdownGenProcs();shutdownFinalProcs(shutdownMDS);}
void Main::restoreReadyForSync(){replace(mTxPoWProcessor,newProcessor());sqlRequired(MinimaDB::getDB()).loadDBsForRestoreSync();}
void Main::archiveResetReady(bool resetWallet,bool resetCascadeTree){
    mRestoring=true;shutdownGenProcs();auto sql=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB();sqlRequired(sql).saveDB(false);if(resetCascadeTree)removeFile(sql->getSQLFile());
    auto archive=sqlRequired(MinimaDB::getDB()).getArchive();sqlRequired(archive).saveDB(false);removeFile(archive->getSQLFile());
    if(resetWallet){sqlRequired(wallet()).saveDB(false);removeFile(wallet()->getSQLFile());}sqlRequired(MinimaDB::getDB()).loadArchiveAndTxPoWDB(resetWallet);
    if(resetCascadeTree){sqlRequired(MinimaDB::getDB()).resetCascadeAndTxPoWTree();log("Deleting cascade..");auto file=sqlRequired(MinimaDB::getDB()).getCascadeFile();std::error_code error;if(std::filesystem::exists(file,error))std::filesystem::remove(file,error);}
}
void Main::resetMemFull(){sqlRequired(MinimaDB::getDB()).fullDBRestartMemFree();sqlRequired(getTxPoWProcessor()).stopMessageProcessor();getTxPoWProcessor()->waitToShutDown();replace(mTxPoWProcessor,newProcessor());collectMemory();}
void Main::restartNIO(){
    if(mShuttingdown)return;sqlRequired(MinimaDB::getDB()).readLock(true);
    try{
        try{log("Network Shutdown started..");sqlRequired(getNetworkManager()).shutdownNetwork();int64_t waited=0;while(!getNetworkManager()->isShutDownComplete()){mServices.sleep(250);waited+=250;if(waited>10000){getNetworkManager()->hardShutDown();break;}}
            log("Network Shutdown complete.. restart in 5 seconds");mServices.sleep(5000);replace(mNetwork,newNetwork());log("Network restarted..");}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){log("[!] Error restarting Network.. Restart Minima!");}
    }catch(...){sqlRequired(MinimaDB::getDB()).readLock(false);throw;}sqlRequired(MinimaDB::getDB()).readLock(false);
}
void Main::PostNotifyEvent(const std::string& event,std::shared_ptr<json::JSONObject> data){PostNotifyEvent(std::optional<std::string>(event),std::move(data),std::string("*"));}
void Main::PostNotifyEvent(const std::optional<std::string>& event,std::shared_ptr<json::JSONObject> data,const std::optional<std::string>& to){
    auto notify=std::make_shared<json::JSONObject>();notify->put("event",event?json::JSONValue(*event):json::JSONValue());notify->put("data",jsonObject(data));
    if(!to)throw JavaNullPointerException("null notification target");if(*to=="*"){if(getNotifyManager())sqlRequired(getNotifyManager()).PostEvent(notify);}
}
void Main::NotifyMainListenerOnly(const std::optional<std::string>& event){
    if(getMinimaListener()){auto notify=std::make_shared<json::JSONObject>();notify->put("event",event?json::JSONValue(*event):json::JSONValue());notify->put("data",json::JSONObject());auto message=std::make_shared<Message>(NotifyManager::NOTIFY_POST);message->addObject("notify",MessageObject(notify));sqlRequired(getMinimaListener()).processMessage(message);}
}
void Main::NotifyMainListenerOnly(const std::optional<std::string>& event,std::shared_ptr<json::JSONObject> data){
    try{if(getMinimaListener()){auto notify=std::make_shared<json::JSONObject>();notify->put("event",event?json::JSONValue(*event):json::JSONValue());notify->put("data",jsonObject(data));auto message=std::make_shared<Message>(NotifyManager::NOTIFY_POST);message->addObject("notify",MessageObject(notify));sqlRequired(getMinimaListener()).processMessage(message);}}
    catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){}
}
void Main::processMessage(std::shared_ptr<Message> message){
    if(mShuttingdown||mRestoring)return;auto type=sqlRequired(message).getMessageType();if(!type)throw JavaNullPointerException("null Main message type");
    if(*type==MAIN_TXPOWMINED){auto pow=powObject(*message);if(!sqlRequired(pow).isTransaction()&&!pow->isBlock())return;if(!GeneralParams::TEST_PARAMS&&pow->isBlock())log("You found a block! "+pow->getBlockNumber().toString()+" "+pow->getTxPoWID());auto data=std::make_shared<MiniData>(NIOManager::createNIOMessage(NIOMessage::MSG_TXPOW,*pow));auto incoming=std::make_shared<Message>(NIOManager::NIO_INCOMINGMSG);incoming->addString("uid","0x00").addObject("data",MessageObject(data,std::function<std::string(const MiniData&)>([](const MiniData& d){return d.to0xString();})));sqlRequired(getNIOManager()).PostMessage(incoming);}
    else if(*type==MAIN_SYSTEMCLEAN){schedule(SYSTEMCLEAN_TIMER,MAIN_SYSTEMCLEAN);collectMemory();}
    else if(*type==MAIN_CLEANDB_RAM){schedule(CLEANDB_RAM_TIMER,MAIN_CLEANDB_RAM);sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).cleanDBRAM();sqlRequired(MinimaDB::getDB()).saveState();sqlRequired(MinimaDB::getDB()).refreshSQLDB();if(!sqlRequired(tree()).getRoot())return;auto height=tree()->getRoot()->getBlockNumber().getAsLong();sqlRequired(CoinDB::getTxPoWTreeCoinDB()).clearOldCoins(subtract(height,200));CoinDB::getTxPoWTreeCoinDB()->closeAndReopen();}
    else if(*type==MAIN_AUTOBACKUP_MYSQL){
        auto prefs=user();if(MYSQL_IMPORTING_NO_ACTION){if(sqlRequired(prefs).getAutoBackupMySQL())log("Skipping MySQL Backup as importing data already..");schedule(MAIN_AUTOBACKUP_MYSQL_TIMER,MAIN_AUTOBACKUP_MYSQL);return;}
        try{for(int coins=0;coins<2;++coins){if(!(coins?sqlRequired(prefs).getAutoBackupMySQLCoins():sqlRequired(prefs).getAutoBackupMySQL()))continue;
            auto command=std::string(coins?"mysqlcoins":"mysql")+" host:"+prefs->getAutoMySQLHost()+" database:"+prefs->getAutoMySQLDB()+" user:"+prefs->getAutoMySQLUser()+" password:"+prefs->getAutoMySQLPassword()+" action:update";
            auto results=CommandRunner::getRunner().runMultiCommand(command);auto& json=commands::raw_detail::object(results.get(0));auto status=commands::command_result::booleanValue(json.get("status"));auto label=std::string(coins?"MYSQLCOINS":"MYSQL");if(!status)log("[ERROR] "+label+" AUTOBACKUP "+commands::raw_detail::stringText(json.get("error")));else log(label+" AUTOBACKUP OK "+commands::raw_detail::object(json.get("response")).toString());}}
        catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){logError(error);}schedule(MAIN_AUTOBACKUP_MYSQL_TIMER,MAIN_AUTOBACKUP_MYSQL);
    }
    else if(*type==MAIN_AUTOBACKUP_TXPOW){if(MYSQL_IMPORTING_NO_ACTION)return;if(GeneralParams::MYSQL_STORE_ALLTXPOW){auto prefs=user();if(sqlRequired(prefs).getAutoBackupMySQL()){auto pow=powObject(*message);auto host=prefs->getAutoMySQLHost();auto database=prefs->getAutoMySQLDB();auto username=prefs->getAutoMySQLUser();auto password=prefs->getAutoMySQLPassword();MySQLConnect connection(host,database,username,password);connection.init();bool status=connection.saveTxPoW(pow.get());connection.shutdown();if(!status)log("[ERROR] MYSQL TXPOW AUTOBACKUP  host:"+prefs->getAutoMySQLHost()+" user:"+prefs->getAutoMySQLUser()+" db:"+prefs->getAutoMySQLDB());}}}
    else if(*type==MAIN_CLEANDB_SQL){schedule(CLEANDB_SQL_TIMER,MAIN_CLEANDB_SQL);sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).cleanDBSQL();sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).checkForCleanDB();}
    else if(*type==MAIN_DO_RESCUE){auto host=message->exists("host")?message->getString("host"):std::optional<std::string>(GeneralParams::RESCUE_MEGAMMR_NODE);if(!host)throw JavaNullPointerException("null rescue host");if(!host->empty()){log("Running MegaMMR Sync from Rescue Node "+*host);mInitKeysCreated=true;auto result=CommandRunner::getRunner().runSingleCommand("megammrsync action:resync host:"+*host);log(result.toString());mServices.halt(0);}else log("NO MegaMMR Sync Rescue Node specified..");}
    else if(*type==MAIN_PULSE){schedule(GeneralParams::USER_PULSE_FREQ,MAIN_PULSE);if(GeneralParams::TXBLOCK_NODE)return;NIOManager::sendNetworkMessageAll(NIOMessage::MSG_PULSE,Pulse::createPulse());}
    else if(*type==MAIN_NEWBLOCK){auto pow=powObject(*message);json::JSONObject data;data.put("txpow",sqlRequired(pow).toJSON());PostNotifyEvent("NEWBLOCK",data);}
    else if(*type==MAIN_BALANCE)PostNotifyEvent("NEWBALANCE",json::JSONObject());
    else if(*type==MAIN_MINING){auto pow=powObject(*message);bool starting=message->getBoolean("starting");json::JSONObject data;data.put("txpow",sqlRequired(pow).toJSON());data.put("mining",starting);PostNotifyEvent("MINING",data);}
    else if(*type==MAIN_NETRESTART){log("[!] MAIN restart networking..");log("Disconnect all peers");sqlRequired(mainOwner()->getNIOManager()).PostMessage(NIOManager::NIO_DISCONNECTALL);log("Wait 10 seconds..");mServices.sleep(10000);sqlRequired(getTxPoWProcessor()).resetFirstIBDTimer();p2p::P2PFunctions::clearInvalidPeers();NIOMessage::mHaveSentIBDRecently.clear();restartNIO();}
    else if(*type==MAIN_AUTOBACKUP){schedule(AUTOBACKUP_TIMER,MAIN_AUTOBACKUP);if(sqlRequired(user()).isAutoBackup())log("AUTOBACKUP : "+CommandRunner::getRunner().runMultiCommand("backup").toString());auto rate=TxPoWMiner::calculateHashSpeed(MiniNumber("250000"));sqlRequired(user()).setHashRate(rate);log("Re-Calculate device hash rate : "+rate.div(MiniNumber::MILLION).setSignificantDigits(4).toString()+" MHs");}
    else if(*type==MAIN_NETRESET){sqlRequired(sqlRequired(mainOwner()->getNIOManager()).getTrafficListener()).reset();schedule(NETRESET_TIMER,MAIN_NETRESET);}
    else if(*type==MAIN_SHUTDOWN)shutdown();
    else if(*type==MAIN_INIT_KEYS){if(!mInitKeysCreated){try{mInitKeysCreated=sqlRequired(wallet()).initDefaultKeys(8);if(mInitKeysCreated)log("All default getaddress keys created..");}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){logError(error);}}if(!mInitKeysCreated)schedule(INIT_KEYS_TIMER,MAIN_INIT_KEYS);}
    else if(*type==MAIN_CHECKER){schedule(CHECKER_TIMER,MAIN_CHECKER);auto tip=sqlRequired(tree()).getTip();if(!tip){log("No tip found in Main Checker..");return;}if(tip->getTxPoW().getTxPoWIDData().isEqual(mOldTip))log("Warning : Chain tip hasn't changed in 180 seconds "+tip->getTxPoW().getTxPoWID()+" "+tip->getTxPoW().getBlockNumber().toString());mOldTip=tip->getTxPoW().getTxPoWIDData();NIOManager::sendNetworkMessageAll(NIOMessage::MSG_PING,tip->getTxPoW().getTxPoWIDData());}
    else if(*type==MAIN_P2PNETMDS_CHECKER){schedule(P2PNETMDS_TIMER,MAIN_P2PNETMDS_CHECKER);bool sent=false;if(GeneralParams::P2P_ENABLED&&p2p::P2PFunctions::isNetAvailable()){auto now=mServices.now();auto tip=sqlRequired(tree()).getTip();if(!tip)return;if(subtract(now,tip->getTxPoW().getTimeMilli().getAsLong())>1800000){log("[!] Chain Tip too far behind.. restart Networking!");sent=true;mainOwner()->PostMessage(MAIN_NETRESTART);}}if(GeneralParams::P2P_ENABLED&&sqlRequired(sqlRequired(mainOwner()->getNetworkManager()).getP2PManager()).getSize()>50){log("[!] P2P Message Overload - Restart");if(!sent)mainOwner()->PostMessage(MAIN_NETRESTART);}p2p::P2PFunctions::clearInvalidPeers();NIOMessage::mHaveSentIBDRecently.clear();}
    else if(*type==MAIN_LOAD_ALL_KEYS){NotifyMainListenerOnly("LOAD_ALL_KEYS_START");while(!mInitKeysCreated){mInitKeysCreated=sqlRequired(wallet()).initDefaultKeys(1);auto data=std::make_shared<json::JSONObject>();data->put("keys",sqlRequired(wallet()).getDefaultKeysNumber());NotifyMainListenerOnly("LOAD_ALL_KEYS_VALUE",data);}sqlRequired(wallet()).updateAllKeyUses(message->getInteger("keyuses"));auto data=std::make_shared<json::JSONObject>();data->put("keys",sqlRequired(wallet()).getDefaultKeysNumber());NotifyMainListenerOnly("LOAD_ALL_KEYS_FINISH",data);}
    else if(*type==MAIN_CALLCHECKER)log(std::string("MAIN Checker Call Recieved.. timer:")+(message->getBoolean("timer",false)?"true":"false"));
}
}
