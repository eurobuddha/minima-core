#include "ArchiveFixture.hpp"
#include "CoinFixture.hpp"
#include "SqlRecordingFixture.hpp"
#include "org/minima/system/brains/TxPoWMiner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::json;
using minima::sql_fixture::object;using minima::sql_fixture::array;
namespace {
int64_t number(const JSONObject& v,const std::string& key,int64_t fallback=0){return v.containsKey(key)?std::stoll(v.get(key).toJSONString()):fallback;}
bool flag(const JSONObject& v,const std::string& key){return v.containsKey(key)&&v.get(key).asBool();}
class Miner : public TxPoWMiner {
public:
    explicit Miner(TxPoWMinerServices s):TxPoWMiner(std::move(s)){}
    using TxPoWMiner::processMessage;using MessageStack::getNextMessage;
    std::vector<std::shared_ptr<TimerMessage>> timers;
    void PostTimerMessage(const std::shared_ptr<TimerMessage>& t) override{timers.push_back(t);}
    std::deque<std::shared_ptr<Message>> queue(){std::lock_guard lock(mLock);return mMessages;}
};
JSONValue state(const std::shared_ptr<TxPoW>& t){if(!t)return {};JSONObject out;out.put("wire",MiniData(t->serialise()).to0xString());out.put("id",t->getTxPoWID());out.put("nonce",t->getNonce().toString());out.put("size",t->getSizeinBytes());out.put("transaction",t->isTransaction());return out;}
JSONObject run(const JSONObject& input){
    std::vector<std::shared_ptr<TxPoW>> txs;
    for(const auto& raw:array(input.get("txs")).elements()){
        const auto& spec=object(raw);auto t=support::archiveFixtureTxPoW(int(number(spec,"seed",1)),0);
        for(const auto* key:{"inputs","burn"})if(spec.containsKey(key))for(const auto& index:array(spec.get(key)).elements())(std::string(key)=="inputs"?t->getTransaction():t->getBurnTransaction()).addInput(support::coinFixture(int(std::stoll(index.toJSONString()))));
        if(spec.containsKey("difficulty"))t->setTxDifficulty(MiniData::fromHex(spec.get("difficulty").asString()));
        t->setHeaderBodyHash();t->calculateTXPOWID();txs.push_back(t);
    }
    auto identity=[&](auto t){if(!t)return -1;auto it=std::find(txs.begin(),txs.end(),t);return it==txs.end()?-2:int(it-txs.begin());};
    auto describe=[&](auto message){JSONObject out;out.put("type",message->getMessageType()?JSONValue(*message->getMessageType()):JSONValue());for(const auto* key:{"starting","automine"})if(message->exists(key))out.put(key,message->getBoolean(key));auto object=message->getObject("txpow");auto t=object.isNull()?nullptr:object.template as<std::shared_ptr<TxPoW>>();out.put("tx",identity(t));out.put("state",state(t));return out;};
    JSONArray events;std::vector<std::shared_ptr<Message>> posted;int calls=0,failAt=0;int64_t delay=number(input,"delay",50000);
    TxPoWMinerServices services;services.releaseLocks=[]{};services.automineTimer=[&]{return delay;};services.getTip=[]()->std::shared_ptr<TxPoWTreeNode>{return nullptr;};
    services.postMain=[&](auto msg){events.add(describe(msg));posted.push_back(msg);if(++calls==failAt)throw std::runtime_error("fixture post failure");};
    auto miner=std::make_shared<Miner>(std::move(services));JSONArray outputs;
    for(const auto& raw:array(input.get("ops")).elements()){
        const auto& op=object(raw);events=JSONArray();calls=0;failAt=int(number(op,"failAt"));size_t priorTimers=miner->timers.size();GeneralParams::MINING_LOGS=flag(op,"logs");GeneralParams::TXBLOCK_NODE=flag(op,"blockNode");
        JSONObject out;out.put("before",TimerMessage::currentTimeMillis());std::ostringstream log;auto prior=std::clog.rdbuf(log.rdbuf());
        auto t=[&]()->std::shared_ptr<TxPoW>{int index=int(number(op,"tx"));return index<0?nullptr:txs.at(size_t(index));};
        try{JSONValue result;auto cmd=op.get("cmd").asString();
            if(cmd=="queue")miner->mineTxPoWAsync(t());
            else if(cmd=="next")miner->processMessage(miner->getNextMessage());
            else if(cmd=="sync")result=miner->MineMaxTxPoW(flag(op,"maxima"),t(),number(op,"limit",100000),!op.containsKey("post")||flag(op,"post"));
            else if(cmd=="process"){auto msg=std::make_shared<Message>(TxPoWMiner::TXPOWMINER_MINETXPOW);msg->addObject("txpow",MessageObject(t()));if(op.containsKey("automine"))msg->addBoolean("automine",flag(op,"automine"));miner->processMessage(msg);}
            else if(cmd=="stop")miner->stopMessageProcessor();
            else if(cmd=="pulse")miner->processMessage(std::make_shared<Message>(TxPoWMiner::TXPOWMINER_MINEPULSE));
            else if(cmd=="block")miner->processMessage(std::make_shared<Message>(TxPoWMiner::TXPOWMINER_TXBLOCKMINER));
            else if(cmd=="unknown")miner->processMessage(std::make_shared<Message>("unknown"));
            else if(cmd=="clearBody")t()->clearBody();
            else if(cmd=="difficulty")t()->setTxDifficulty(MiniData::fromHex(op.get("value").asString()));
            else if(cmd=="input")t()->getTransaction().addInput(support::coinFixture(int(number(op,"coin"))));
            else if(cmd=="check")result=miner->checkForMiningCoin(op.get("id").isNull()?std::nullopt:std::optional(op.get("id").asString()));
            else throw std::invalid_argument(cmd);out.put("value",result);
        }catch(const std::exception& e){out.put("exception",javaExceptionText(e));}
        std::clog.rdbuf(prior);JSONArray logs;std::string line;std::istringstream lines(log.str());while(std::getline(lines,line))logs.add(line);out.put("logs",logs);out.put("events",events);
        JSONArray timers;for(size_t i=priorTimers;i<miner->timers.size();++i){JSONObject row;row.put("type",*miner->timers[i]->getMessageType());row.put("deadline",miner->timers[i]->getTimer());timers.add(row);}out.put("timers",timers);
        JSONArray pending,retained,states,coins;for(const auto& msg:miner->queue())pending.add(describe(msg));for(const auto& msg:posted)retained.add(describe(msg));for(const auto& tx:txs)states.add(state(tx));for(int i=0;i<6;++i)coins.add(miner->checkForMiningCoin(support::coinFixture(i).getCoinID().to0xString()));
        out.put("pending",pending);out.put("retained",retained);out.put("states",states);out.put("coins",coins);out.put("running",miner->isRunning());out.put("after",TimerMessage::currentTimeMillis());outputs.add(out);
    }
    JSONObject out;out.put("ops",outputs);return out;
}
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
