#pragma once
#include "SqlRecordingFixture.hpp"
#include "org/minima/system/brains/TxPoWProcessor.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
namespace minima::processor_fixture {
using namespace json;
inline JSONArray events;
inline int calls=0,failAt=-1;
inline bool shutdown=false,restore=false,relevant=false,sync=false;
inline void event(const std::string& name,std::initializer_list<JSONValue> args={}){JSONArray row;row.add(name);for(const auto& arg:args)row.add(arg);events.add(row);if(++calls==failAt)throw std::runtime_error("fixture receiver failure");}
inline SqlDBServices sql(){return {[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{throw std::logic_error("Processor fixture must not open SQL");},[]{return false;},{}};}
class Pool:public TxPoWDB {
public:
    TxPoWs order;
    Pool():TxPoWDB({sql(),[](auto){},[]{return ChainServices{};}}){}
    bool addTxPoW(std::shared_ptr<TxPoW> txpow) override {event("add",{sqlRequired(txpow).getTxPoWID()});if(!mRamDB->exists(txpow->getTxPoWID())){mRamDB->addTxPoW(txpow);order.push_back(txpow);}return relevant;}
    std::shared_ptr<TxPoW> getTxPoW(const std::optional<std::string>& id) override {event("get",{id?JSONValue(*id):JSONValue()});return mRamDB->getTxPoW(id.value());}
    bool exists(const std::optional<std::string>& id) override {event("exists",{id?JSONValue(*id):JSONValue()});return mRamDB->exists(id.value());}
    TxPoWs getChildBlocks(const std::optional<std::string>& id) override {event("children",{id?JSONValue(*id):JSONValue()});TxPoWs result;for(auto t:order)if(t->isBlock()&&t->getParentID().to0xString()==id)result.push_back(t);return result;}
    void clearMainChainTxns() override {event("clearChain");TxPoWDB::clearMainChainTxns();}
    void setOnMainChain(const std::optional<std::string>& id) override {event("onChain",{id?JSONValue(*id):JSONValue()});TxPoWDB::setOnMainChain(id);}
    void setInCascade(const std::optional<std::string>& id) override {event("inCascade",{id?JSONValue(*id):JSONValue()});TxPoWDB::setInCascade(id);}
};
class Archive:public ArchiveManager {
public:
    Blocks saved;
    explicit Archive():ArchiveManager(sql(),[]{return shutdown;}){}
    bool saveBlock(const TxBlock* block) override {if(!block)throw JavaNullPointerException("null archive block");event("save",{block->getTxPoW().getTxPoWID()});return true;}
    std::shared_ptr<TxBlock> last;
    std::shared_ptr<TxBlock> loadLastBlock() override {event("last");return last;}
    int32_t checkForCleanDB() override {event("clean");return 0;}
    void checkCascadeRequired(const Cascade* cascade) override {event("checkCascade",{cascade?cascade->getLength():-1});}
};
inline MinimaDBServices dbServices(){return {sql(),[]{return shutdown;},[](auto){},[](auto){},[](const auto&,const auto&){},[](bool,const auto&){},[](int){throw std::logic_error("unexpected halt");},[]{return "processor-fixture";}};}
class DB:public MinimaDB {
public:
    std::shared_ptr<Pool> pool=std::make_shared<Pool>();
    std::shared_ptr<Archive> archive=std::make_shared<Archive>();
    DB():MinimaDB(dbServices()){mTxPoWDB=pool;mArchive=archive;}
    void writeLock(bool lock) override {event("writeLock",{lock});MinimaDB::writeLock(lock);}
    void setIBDCascade(std::shared_ptr<Cascade> value) override {event("setCascade",{value?value->getLength():-1});MinimaDB::setIBDCascade(value);}
};
class Processor:public TxPoWProcessor {
public:
    explicit Processor(TxPoWProcessorServices services):TxPoWProcessor(std::move(services)){}
    using TxPoWProcessor::processMessage;using MessageStack::getNextMessage;
    std::deque<std::shared_ptr<Message>> queue(){std::lock_guard lock(mLock);return mMessages;}
};
inline TxPoWProcessorServices services(const std::shared_ptr<DB>& db){
    TxPoWProcessorServices result;result.releaseLocks=[db]{db->safeReleaseReadWriteLock();};result.getDB=[db]{return db;};
    result.isShuttingDown=[]{return shutdown;};result.isShuttingDownOrRestoring=[]{return shutdown||restore;};
    result.setSyncIBD=[](bool value){event("sync",{value});sync=value;};
    result.postMain=[](auto msg){auto value=msg->getObject("txpow");auto tx=value.isNull()?nullptr:value.template as<std::shared_ptr<TxPoW>>();event("main",{msg->getMessageType().value_or("null"),tx?JSONValue(tx->getTxPoWID()):JSONValue()});};
    result.postNotifyEvent=[](const auto& name,const auto& data){event("notify",{name,data});};
    result.postNIO=[](auto msg){auto uid=msg->getString("client");event("nio",{msg->getMessageType().value_or("null"),uid?JSONValue(*uid):JSONValue()});};
    result.disconnect=[](const auto& uid,bool remove){event("disconnect",{uid?JSONValue(*uid):JSONValue(),remove});};
    result.send=[](const auto& uid,const auto& type,const auto& data){if(!uid)throw JavaNullPointerException("null send UID");event("send",{*uid,NIOManager::createNIOMessage(type,data).to0xString()});};
    result.sendAll=[send=result.send](const auto& type,const auto& data){send(std::string(),type,data);};return result;
}
inline void bind(const std::shared_ptr<DB>& db){
    ChainServices chain;chain.isAddressRelevant=[](const auto&){return false;};chain.isKeyRelevant=[](const auto&){return false;};chain.checkCoinNotify=[](const auto&){return false;};
    chain.postNotifyEvent=[](const auto& name,const auto& data){event("notify",{name,data});};chain.postMessage=[](const auto& name){event("main",{name,JSONValue()});};
    chain.getAllRelevantUnspentCoins=TxPoWSearcher::getAllRelevantUnspentCoins;
    chain.addOnChainTxPoW=[](const auto& block,const auto& number,const auto& tx){event("onChainDB",{block,number.toString(),tx});};TxPoWTreeNode::services=chain;
    TxPoWChecker::services.getCascadeTip=[db]{return db->getCascade()->getTip();};
    TxPoWGenerator::services.getRoot=[db]{return db->getTxPoWTree()->getRoot();};
}
inline void clearBindings(){TxPoWTreeNode::services={};TxPoWChecker::services={};TxPoWGenerator::services={};}
}
