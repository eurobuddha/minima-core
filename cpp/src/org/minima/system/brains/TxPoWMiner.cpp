#include "TxPoWMiner.hpp"
#include "TxPoWGenerator.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <bit>
#include <iostream>
namespace minima {
namespace {
const MiniNumber& startNonce(){static const MiniNumber value("100000000000000000.00000000000000000000000000000000000000001");return value;}
template<class T> T& required(const std::shared_ptr<T>& value){if(!value)throw JavaNullPointerException("null miner receiver");return *value;}
int64_t elapsed(int64_t now,int64_t start){return std::bit_cast<int64_t>(uint64_t(now)-uint64_t(start));}
void nextNonce(std::vector<uint8_t>& header,BigInt& nonce){
    auto bytes=nonce.toJavaBytes();nonce=nonce+BigInt::one();
    if(header.size()<4||bytes.size()>header.size()-4)throw std::out_of_range("Nonce exceeds header array");
    std::copy(bytes.begin(),bytes.end(),header.begin()+4);
}
MiniNumber readNonce(const std::vector<uint8_t>& header){
    auto value=TxHeader::convertMiniDataVersion(MiniData(header));
    if(!value)throw JavaNullPointerException("null decoded mining header");return value->mNonce;
}
std::shared_ptr<Message> txMessage(const std::string& type,const std::shared_ptr<TxPoW>& txpow){
    auto message=std::make_shared<Message>(type);message->addObject("txpow",MessageObject(txpow));return message;
}
}
TxPoWMiner::TxPoWMiner(TxPoWMinerServices services):MessageProcessor("MINER",services.releaseLocks),mServices(std::move(services)){
    if(!mServices.postMain||!mServices.automineTimer||!mServices.now)throw std::invalid_argument("TxPoWMiner requires Main and clock services");
    if(!mServices.getTip)mServices.getTip=[]{return required(required(MinimaDB::getDB()).getTxPoWTree()).getTip();};
    if(!mServices.generate)mServices.generate=[]{return std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(Transaction(),Witness()));};
    if(!mServices.sendAll)mServices.sendAll=[](const auto& type,auto txpow){NIOManager::sendNetworkMessageAll(type,required(txpow));};
    if(!mServices.log)mServices.log=[](const auto& text){std::clog<<text<<'\n';};
}
std::shared_ptr<TxPoWMiner> TxPoWMiner::create(TxPoWMinerServices services){auto miner=std::shared_ptr<TxPoWMiner>(new TxPoWMiner(std::move(services)));miner->startMessageProcessor();return miner;}
void TxPoWMiner::addMiningCoins(const std::shared_ptr<TxPoW>& txpow){
    auto add=[&](const Transaction& txn){if(!txn.isEmpty())for(const auto& coin:txn.getAllInputs()){
        auto id=coin.getCoinID().to0xString();std::lock_guard lock(mCoinsLock);
        if(std::find(mMiningCoins.begin(),mMiningCoins.end(),id)==mMiningCoins.end())mMiningCoins.push_back(std::move(id));
    }};add(required(txpow).getTransaction());add(txpow->getBurnTransaction());
}
void TxPoWMiner::removeMiningCoins(const std::shared_ptr<TxPoW>& txpow){
    auto remove=[&](const Transaction& txn){if(!txn.isEmpty())for(const auto& coin:txn.getAllInputs()){
        auto id=coin.getCoinID().to0xString();std::lock_guard lock(mCoinsLock);
        auto it=std::find(mMiningCoins.begin(),mMiningCoins.end(),id);if(it!=mMiningCoins.end())mMiningCoins.erase(it);
    }};remove(required(txpow).getTransaction());remove(txpow->getBurnTransaction());
}
bool TxPoWMiner::checkForMiningCoin(const std::optional<std::string>& coinID) const {std::lock_guard lock(mCoinsLock);return coinID&&std::find(mMiningCoins.begin(),mMiningCoins.end(),*coinID)!=mMiningCoins.end();}
void TxPoWMiner::mineTxPoWAsync(std::shared_ptr<TxPoW> txpow){addMiningCoins(txpow);txpow->setHeaderBodyHash();auto message=txMessage(TXPOWMINER_MINETXPOW,txpow);message->addBoolean("automine",false);PostMessage(message);}
void TxPoWMiner::processMessage(std::shared_ptr<Message> message){
    if(required(message).isMessageType(TXPOWMINER_MINETXPOW)){
        auto start=mServices.now();bool automine=false;if(message->exists("automine"))automine=message->getBoolean("automine");
        if(GeneralParams::MINING_LOGS)mServices.log(std::string("MINING TXPOW START auto:")+(automine?"true":"false"));
        auto object=message->getObject("txpow");auto txpow=object.isNull()?nullptr:object.as<std::shared_ptr<TxPoW>>();
        required(txpow).setHeaderBodyHash();txpow->setNonce(startNonce());
        auto mining=std::make_shared<Message>("MAIN_MINING");mining->addBoolean("starting",true).addBoolean("automine",automine).addObject("txpow",MessageObject(txpow));mServices.postMain(mining);
        auto data=txpow->getTxHeader().serialise();MiniNumber finalnonce=MiniNumber::ZERO;BigInt nonce=BigInt::zero();
        while(isRunning()){
            nextNonce(data,nonce);MiniData hash(Crypto::getInstance().hashData(data));
            if(hash.isLess(txpow->getTxnDifficulty())){finalnonce=readNonce(data);break;}
        }
        txpow->setNonce(finalnonce);txpow->calculateTXPOWID();
        if(txpow->isTransaction())mServices.log("ASYNC Transaction Mined : "+txpow->getTxPoWID());
        auto end=std::make_shared<Message>("MAIN_MINING");end->addBoolean("starting",false);
        // The pinned source writes automine to the START message again.
        mining->addBoolean("automine",automine);end->addObject("txpow",MessageObject(txpow));mServices.postMain(end);
        mServices.postMain(txMessage("MAIN_TXPOWMINED",txpow));removeMiningCoins(txpow);
        if(automine){PostTimerMessage(std::make_shared<TimerMessage>(mServices.automineTimer(),TXPOWMINER_MINEPULSE));auto delay=mServices.automineTimer()/2;PostTimerMessage(std::make_shared<TimerMessage>(delay,TXPOWMINER_TXBLOCKMINER));}
        if(GeneralParams::MINING_LOGS)mServices.log("MINING TXPOW FINISHED time:"+std::to_string(elapsed(mServices.now(),start)));
    }else if(message->isMessageType(TXPOWMINER_MINEPULSE)){
        if(GeneralParams::TXBLOCK_NODE)return;
        if(mServices.getTip()){auto txpow=mServices.generate();auto mining=txMessage(TXPOWMINER_MINETXPOW,txpow);mining->addBoolean("automine",true);PostMessage(mining);}
        else PostTimerMessage(std::make_shared<TimerMessage>(30000,TXPOWMINER_MINEPULSE));
    }else if(message->isMessageType(TXPOWMINER_TXBLOCKMINER)){
        if(GeneralParams::TXBLOCK_NODE)return;auto txpow=mServices.generate();mServices.sendAll(NIOMessage::MSG_TXBLOCKMINE,txpow);
    }
}
bool TxPoWMiner::MineMaxTxPoW(bool maxima,std::shared_ptr<TxPoW> txpow,int64_t timeLimit,bool post){
    auto start=mServices.now();addMiningCoins(txpow);
    if(GeneralParams::MINING_LOGS&&maxima)mServices.log("MINING MAXIMA START");
    txpow->setHeaderBodyHash();txpow->setNonce(startNonce());auto data=txpow->getTxHeader().serialise();
    MiniNumber finalnonce=MiniNumber::ZERO;BigInt nonce=BigInt::zero();int counter=0;
    while(true){
        nextNonce(data,nonce);MiniData hash(Crypto::getInstance().hashData(data));
        if(hash.isLess(txpow->getTxnDifficulty())){finalnonce=readNonce(data);break;}
        if(++counter>10000){if(elapsed(mServices.now(),start)>timeLimit)return false;counter=0;}
    }
    txpow->setNonce(finalnonce);txpow->calculateTXPOWID();
    if(txpow->isTransaction())mServices.log("SYNC Transaction Mined : "+txpow->getTxPoWID());
    if(post)mServices.postMain(txMessage("MAIN_TXPOWMINED",txpow));removeMiningCoins(txpow);
    if(GeneralParams::MINING_LOGS&&maxima)mServices.log("MINING MAXIMA FINISHED : "+std::to_string(elapsed(mServices.now(),start)));
    return true;
}
MiniNumber TxPoWMiner::calculateHashRateOld(const MiniNumber& hashes,const std::function<int64_t()>& now){
    int count=hashes.getAsInt();auto start=now();MiniData data=MiniData::getRandomData(512);
    for(int i=0;i<count;++i)data=Crypto::getInstance().hashObject(data);
    auto time=elapsed(now(),start);if(time<=0)time=1;return hashes.div(MiniNumber(time).div(MiniNumber::THOUSAND));
}
MiniNumber TxPoWMiner::calculateHashSpeed(const MiniNumber& hashes,const std::function<int64_t()>& now){
    int count=hashes.getAsInt();auto start=now();TxPoW txpow;txpow.setHeaderBodyHash();txpow.setNonce(startNonce());
    auto data=txpow.getTxHeader().serialise();BigInt nonce=BigInt::zero();
    for(int i=0;i<count;++i){nextNonce(data,nonce);(void)Crypto::getInstance().hashData(data);}
    auto time=elapsed(now(),start);if(time<=0)time=1;return hashes.div(MiniNumber(time).div(MiniNumber::THOUSAND));
}
}
