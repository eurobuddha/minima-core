#include "TxPoWProcessorFixture.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::json;using namespace minima::processor_fixture;
using minima::sql_fixture::object;using minima::sql_fixture::array;using minima::sql_fixture::integer;using minima::sql_fixture::string;
namespace {
bool flag(const JSONObject& v,const std::string& key){return v.containsKey(key)&&v.get(key).asBool();}
JSONObject run(const JSONObject& input){
    shutdown=restore=relevant=sync=false;calls=0;failAt=-1;events=JSONArray();
    GeneralParams::USE_SQL_COINDB=false;GeneralParams::BLOCK_LOGS=false;GeneralParams::IBDSYNC_LOGS=false;GeneralParams::TXBLOCK_NODE=false;GeneralParams::TEST_PARAMS=false;GeneralParams::NOTIFY_ALL_TXPOW=false;GeneralParams::IS_MEGAMMR=false;GeneralParams::NO_SYNC_IBD=false;GeneralParams::DB_IGNORE_LOCKS=false;
    GlobalParams::MINIMA_CASCADE_START_DEPTH=MiniNumber(integer(input,"depth",1000));GlobalParams::MINIMA_CASCADE_FREQUENCY=MiniNumber(integer(input,"frequency",1000));
    auto db=std::make_shared<DB>();processor_fixture::bind(db);struct Clear{~Clear(){clearBindings();}}clear;
    std::vector<std::shared_ptr<TxBlock>> blocks;std::vector<std::shared_ptr<TxPoW>> txs;
    for(const auto& raw:array(input.get("blocks")).elements()){auto bytes=MiniData::fromHex(raw.asString());size_t offset=0;auto block=std::make_shared<TxBlock>(TxBlock::deserialise(bytes.bytes(),offset));blocks.push_back(block);txs.push_back(block->getSharedTxPoW());}
    for(const auto& raw:array(input.get("txs")).elements()){auto bytes=MiniData::fromHex(raw.asString());size_t offset=0;txs.push_back(std::make_shared<TxPoW>(TxPoW::deserialise(bytes.bytes(),offset)));}
    auto processor=std::make_shared<Processor>(services(db));JSONArray rows;
    auto describe=[&](const auto& msg){JSONObject out;out.put("type",msg->getMessageType().value_or("null"));for(auto key:{"uid","notifyfinish"})if(msg->exists(key)){if(std::string(key)=="uid")out.put(key,*msg->getString(key));else out.put(key,msg->getBoolean(key));}return out;};
    for(const auto& raw:array(input.get("ops")).elements()){
        const auto& op=object(raw);events=JSONArray();calls=0;failAt=int(integer(op,"fail",-1));
        for(auto [key,target]:std::initializer_list<std::pair<const char*,bool*>>{{"shutdown",&shutdown},{"restore",&restore},{"relevant",&relevant},{"blockNode",&GeneralParams::TXBLOCK_NODE},{"test",&GeneralParams::TEST_PARAMS},{"notifyAll",&GeneralParams::NOTIFY_ALL_TXPOW},{"mega",&GeneralParams::IS_MEGAMMR},{"noSync",&GeneralParams::NO_SYNC_IBD}})if(op.containsKey(key))*target=flag(op,key);
        if(op.containsKey("logs"))GeneralParams::BLOCK_LOGS=flag(op,"logs");
        if(op.containsKey("ibdLogs"))GeneralParams::IBDSYNC_LOGS=flag(op,"ibdLogs");
        auto tx=[&](){int index=int(integer(op,"tx"));return index<0?std::shared_ptr<TxPoW>{}:txs.at(size_t(index));};auto block=[&](){int index=int(integer(op,"tx"));return index<0?std::shared_ptr<TxBlock>{}:blocks.at(size_t(index));};
        auto ibd=[&](){if(flag(op,"nullIBD"))return std::shared_ptr<IBD>{};auto value=std::make_shared<IBD>();if(op.containsKey("list"))for(const auto& rawIndex:array(op.get("list")).elements()){int index=int(std::get<int64_t>(rawIndex.value));value->getTxBlocks()->push_back(index<0?nullptr:blocks.at(size_t(index)));}if(op.containsKey("cascade")){auto c=std::make_shared<Cascade>();int index=int(integer(op,"cascade"));if(index>=0)c->addToTip(*txs.at(size_t(index)));c->cascadeChain();value->setCascade(c);}return value;};
        JSONObject row;row.put("before",TimerMessage::currentTimeMillis());std::ostringstream log;auto previous=std::clog.rdbuf(log.rdbuf());
        try{auto cmd=op.get("cmd").asString();auto uid=op.containsKey("uid")?string(op.get("uid")):std::optional<std::string>("c0");
            if(cmd=="post")processor->postProcessTxPoW(tx());else if(cmd=="postBlock")processor->postProcessTxBlock(block());else if(cmd=="ibd")processor->postProcessIBD(ibd(),uid,flag(op,"override"));else if(cmd=="archive")processor->postProcessArchiveIBD(ibd(),uid);else if(cmd=="sync")processor->postProcessSyncIBD(ibd(),uid);
            else if(cmd=="next")processor->processMessage(processor->getNextMessage());else if(cmd=="root")db->getTxPoWTree()->setRoot(std::make_shared<TxPoWTreeNode>(block()));else if(cmd=="add")db->pool->addTxPoW(tx());else if(cmd=="last")db->archive->last=block();
            else if(cmd=="recalc")processor->onStartUpRecalc();else if(cmd=="ask")processor->askToSyncTxBlocks(uid);else if(cmd=="check")processor->postCheckCall();else if(cmd=="timer")processor->resetFirstIBDTimer();
            else if(cmd=="nonce")tx()->setNonce(MiniNumber(integer(op,"value")));else if(cmd=="time")tx()->setTimeMilli(MiniNumber(integer(op,"value")));else if(cmd=="parent")tx()->setSuperParent(0,MiniData::fromHex("0x01"));else if(cmd=="chain")tx()->getTxHeader().mChainID=MiniData::fromHex("0x01");
            else if(cmd=="height")tx()->setBlockNumber(MiniNumber(integer(op,"value")));else if(cmd=="work")tx()->setBlockDifficulty(MiniData::fromHex("0x01"));else if(cmd=="magic")tx()->getMagic().mCurrentMaxTxnPerBlock()=std::make_shared<MiniNumber>(MiniNumber(3));else if(cmd=="super")tx()->setSuperParent(31,MiniData::fromHex("0x01"));
            else if(cmd=="link")tx()->addBlockTxPOW(txs.at(size_t(integer(op,"other")))->getTxPoWIDData());else if(cmd=="stop")processor->stopMessageProcessor();else if(cmd=="info"){}else throw std::invalid_argument(cmd);
        }catch(const std::exception& e){row.put("error",sqlExceptionText(e));}
        std::clog.rdbuf(previous);JSONArray logs;std::string line;std::istringstream stream(log.str());while(std::getline(stream,line))logs.add(line);row.put("logs",logs);row.put("events",events);
        auto tree=db->getTxPoWTree();row.put("root",tree->getRoot()?JSONValue(tree->getRoot()->getTxPoW().getTxPoWID()):JSONValue());row.put("tip",tree->getTip()?JSONValue(tree->getTip()->getTxPoW().getTxPoWID()):JSONValue());row.put("length",tree->getHeaviestBranchLength());row.put("size",tree->getSize());row.put("cascade",MiniData(db->getCascade()->serialise()).to0xString());row.put("finished",processor->isIBDProcessFinished());row.put("sync",sync);row.put("lock",MinimaDB::mCurrentWriteLockState.load());
        JSONArray nodes,pool,pending;for(auto tx:txs){auto node=tree->findNode(tx->getTxPoWID());JSONArray data;data.add(bool(node));if(node){data.add(node->getSharedTxPoW()==tx);data.add(node->getTxPoW().getNonce().toString());data.add(MiniData(node->getMMR()->serialise()).to0xString());}nodes.add(data);auto entry=db->pool->getCompleteMemPool()->get(tx->getTxPoWID());JSONArray state;state.add(bool(entry));if(entry){state.add((*entry)->getTxPoW()==tx);state.add((*entry)->isOnMainChain());state.add((*entry)->isInCascade());}pool.add(state);}
        for(auto msg:processor->queue())pending.add(describe(msg));row.put("nodes",nodes);row.put("pool",pool);row.put("pending",pending);row.put("after",TimerMessage::currentTimeMillis());rows.add(row);
    }
    JSONObject out;out.put("ops",rows);return out;
}
}
int main(){
    // The source oracle captures MinimaLogger's console output. Retain the
    // existing per-operation stream capture for processor paths still using clog.
    MinimaLogger::Runtime logger;logger.println=[](const auto& full){auto text=unicode::encodeUTF8(full);auto offset=text.find("] : ");if(offset!=std::string::npos){auto body=text.substr(offset+4);if(!body.starts_with("     "))std::clog<<body<<'\n';}};MinimaLogger::setRuntime(logger);
    std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
