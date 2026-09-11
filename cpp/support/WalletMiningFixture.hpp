#pragma once
// Replay only at miner entry: keep generated PRNG length but fix its bytes.
// The actual asynchronous queue and synchronous mining bodies still run.
#include "org/minima/system/commands/txn/txnminepost.hpp"
#include "org/minima/system/commands/search/keys.hpp"
struct WalletMiningFixture {
    class Miner:public TxPoWMiner {
    public:
        explicit Miner(TxPoWMinerServices services):TxPoWMiner(std::move(services)){}
        void mineTxPoWAsync(std::shared_ptr<TxPoW> pow) override {
            enter("async",pow,false,0,true);TxPoWMiner::mineTxPoWAsync(std::move(pow));
        }
        bool MineMaxTxPoW(bool maxima,std::shared_ptr<TxPoW> pow,int64_t limit,bool post=true) override {
            enter("sync",pow,maxima,limit,post);
            auto mode=config->containsKey("mineResult")?config->getString("mineResult"):"real";
            if(mode=="false")return false;if(mode=="throw")throw std::runtime_error("fixture mining failure");
            return TxPoWMiner::MineMaxTxPoW(maxima,std::move(pow),limit,post);
        }
        std::deque<std::shared_ptr<Message>> queue(){std::lock_guard lock(mLock);return mMessages;}
        void processNext(){processMessage(getNextMessage());}
    };
    inline static std::shared_ptr<Miner> miner;
    inline static std::vector<std::shared_ptr<Message>> posted;
    inline static std::vector<std::shared_ptr<TxPoW>> held;
    inline static JSONArray calls;
    static void reset(){commands::keys::postMessage=[](auto){throw JavaNullPointerException("null Main receiver");};miner.reset();posted.clear();held.clear();calls=JSONArray();commands::txnminepost::postMessage=[](auto){throw JavaNullPointerException("null Main receiver");};}
    static void post(std::shared_ptr<Message> message){posted.push_back(message);if(flag("failPost"))throw std::runtime_error("fixture post failure");}
    static void init(){
        commands::txnminepost::postMessage=post;commands::keys::postMessage=post;
        if(flag("nullMiner")){miner.reset();commands::txnutils::getMiner=[]{return std::shared_ptr<TxPoWMiner>{};};return;}
        TxPoWMinerServices services;services.releaseLocks=[]{};services.automineTimer=[]{return int64_t(1000);};services.postMain=post;services.log=[](const auto& text){logs.add(text);};
        miner=std::make_shared<Miner>(services);commands::txnutils::getMiner=[]{return miner;};
    }
    static void enter(const std::string& kind,const std::shared_ptr<TxPoW>& pow,bool maxima,int64_t limit,bool post){
        // PRNG is generated at fixed length32. Keep every other generated field.
        pow->getTxBody()->mPRNG=MiniData(std::vector<uint8_t>(32,0x12));
        auto row=MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test");JSONObject call;
        call.put("kind",kind);call.put("maxima",maxima);call.put("limit",limit);call.put("post",post);
        call.put("wire",MiniData(pow->serialise()).to0xString());call.put("sameTransaction",row&&row->getTransaction()==pow->getSharedTransaction());
        call.put("sameWitness",row&&row->getWitness()==pow->getSharedWitness());calls.add(call);held.push_back(pow);
    }
    static JSONValue describe(const std::shared_ptr<Message>& message){
        JSONObject value;value.put("type",*message->getMessageType());
        for(const auto* key:{"automine","starting"})if(message->exists(key))value.put(key,message->getBoolean(key));
        if(message->exists("keyuses"))value.put("keyuses",message->getInteger("keyuses"));
        auto object=message->getObject("txpow");auto pow=object.isNull()?nullptr:object.as<std::shared_ptr<TxPoW>>();
        value.put("wire",pow?JSONValue(MiniData(pow->serialise()).to0xString()):JSONValue());return value;
    }
    static JSONObject snapshot(){
        JSONObject value;value.put("calls",calls);JSONArray sent,pending,retained,coins;
        for(const auto& message:posted)sent.add(describe(message));if(miner)for(const auto& message:miner->queue())pending.add(describe(message));
        for(const auto& pow:held)retained.add(MiniData(pow->serialise()).to0xString());
        if(miner)for(const auto& pow:held)for(const auto* txn:{&pow->getTransaction(),&pow->getBurnTransaction()})for(const auto txnItems=txn->getAllInputs();const auto& coin:*txnItems)coins.add(miner->checkForMiningCoin(coin.getCoinID().to0xString()));
        value.put("posted",sent);value.put("pending",pending);value.put("held",retained);value.put("coins",coins);return value;
    }
};
