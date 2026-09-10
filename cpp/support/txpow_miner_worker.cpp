#include "ArchiveFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/system/brains/TxPoWMiner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <future>
#include <latch>
#include <iostream>
using namespace minima;
namespace {
void require(bool value,const char* text){if(!value)throw std::runtime_error(text);}
TxPoWMinerServices services(){TxPoWMinerServices s;s.releaseLocks=[]{};s.postMain=[](auto){};s.automineTimer=[]{return int64_t(50000);};s.log=[](const auto&){};return s;}
auto transaction(){auto t=support::archiveFixtureTxPoW(1,0);t->getTransaction().addInput(support::coinFixture(0));return t;}
class Manual : public TxPoWMiner {
public:explicit Manual(TxPoWMinerServices s):TxPoWMiner(std::move(s)){}
    using TxPoWMiner::processMessage;using MessageStack::getNextMessage;
};
}
int main()try{
    GeneralParams::MINING_LOGS=false;GeneralParams::TXBLOCK_NODE=false;
    std::atomic<int> cleaned=0;std::latch completed(16);std::vector<std::shared_ptr<TxPoW>> outputs;
    auto s=services();s.releaseLocks=[&]{++cleaned;completed.count_down();};s.postMain=[&](auto m){if(m->isMessageType("MAIN_TXPOWMINED"))outputs.push_back(m->getObject("txpow").template as<std::shared_ptr<TxPoW>>());};
    auto miner=TxPoWMiner::create(s);std::vector<std::shared_ptr<TxPoW>> inputs;
    for(int i=0;i<16;++i){auto tx=transaction();inputs.push_back(tx);miner->mineTxPoWAsync(tx);}completed.wait();miner->stopMessageProcessor();miner->joinMessageProcessor();
    require(cleaned==16&&outputs==inputs,"Worker output identity/order or finally cleanup");for(auto tx:outputs)require(tx->getTxPoWIDData().isLess(tx->getTxnDifficulty()),"Invalid mined TxPoW");
    require(!miner->checkForMiningCoin("0x01")&&miner->isShutdownComplete(),"Worker reservations/shutdown");
    // Hold the worker inside its start notification, stop and release the outside
    // owner, then let the actual mining loop observe stopped state.
    std::promise<void> entered,resume;auto resumeFuture=resume.get_future().share();std::latch canceledDone(1);auto canceledTx=transaction();int mined=0;
    s=services();s.releaseLocks=[&]{canceledDone.count_down();};s.postMain=[&](auto m){if(m->isMessageType("MAIN_MINING")&&m->getBoolean("starting")){entered.set_value();resumeFuture.wait();}if(m->isMessageType("MAIN_TXPOWMINED"))++mined;};
    auto canceled=TxPoWMiner::create(s);std::weak_ptr<TxPoWMiner> weak=canceled;canceled->mineTxPoWAsync(canceledTx);entered.get_future().wait();canceled->stopMessageProcessor();canceled.reset();require(!weak.expired(),"Lost active miner owner");resume.set_value();canceledDone.wait();if(auto held=weak.lock())held->joinMessageProcessor();
    for(int i=0;i<100&&!weak.expired();++i)std::this_thread::sleep_for(std::chrono::milliseconds(2));require(weak.expired()&&mined==1&&canceledTx->getNonce().isEqual(MiniNumber::ZERO),"Stopped-loop finalization or worker release");
    // A first callback failure must run the MessageProcessor finally cleanup and
    // leave the worker able to mine the following message.
    std::latch recovered(2);int posts=0;s=services();s.releaseLocks=[&]{recovered.count_down();};s.postMain=[&](auto){if(++posts==1)throw std::runtime_error("expected miner callback failure");};auto recovery=TxPoWMiner::create(s);recovery->mineTxPoWAsync(transaction());recovery->mineTxPoWAsync(transaction());recovered.wait();recovery->stopMessageProcessor();recovery->joinMessageProcessor();require(posts==4,"Worker failed to continue after exception");
    // Strict > time limit: equal elapsed time permits another full 10001-failure
    // batch; this test uses real SHA3 and an impossible target.
    int clocks=0;s=services();s.now=[&]{return int64_t(clocks++==0?0:clocks==2?10:11);};auto timed=std::make_shared<Manual>(s);auto impossible=transaction();impossible->setTxDifficulty(MiniData::fromHex("0x00"));require(!timed->MineMaxTxPoW(false,impossible,10,false)&&clocks==3,"Timeout boundary");require(timed->checkForMiningCoin("0x01"),"Source timeout must retain reservation");
    for(const auto* count:{"0","-1","0.5","3.5","4294967296"})for(int64_t elapsed:{int64_t(-1),int64_t(0),int64_t(10)})for(bool old:{false,true}){
        int calls=0;auto clock=[&]{return calls++==0?int64_t(100):100+elapsed;};MiniNumber hashes(count);auto expected=hashes.div(MiniNumber(elapsed<=0?1:elapsed).div(MiniNumber::THOUSAND));auto speed=old?TxPoWMiner::calculateHashRateOld(hashes,clock):TxPoWMiner::calculateHashSpeed(hashes,clock);require(calls==2&&speed.isEqual(expected),"Hash-rate arithmetic/clock");
    }
    int generated=0,broadcasts=0;s=services();auto candidate=transaction();s.getTip=[&]{return std::make_shared<TxPoWTreeNode>(*candidate);};s.generate=[&]{++generated;return candidate;};s.sendAll=[&](const auto& type,auto tx){require(type.getValue()==21&&tx==candidate,"Mining broadcast type/identity");++broadcasts;};auto routing=std::make_shared<Manual>(s);
    routing->processMessage(std::make_shared<Message>(TxPoWMiner::TXPOWMINER_MINEPULSE));auto queued=routing->getNextMessage();require(queued&&queued->getBoolean("automine")&&queued->getObject("txpow").as<std::shared_ptr<TxPoW>>()==candidate,"Generated pulse queue");routing->processMessage(std::make_shared<Message>(TxPoWMiner::TXPOWMINER_TXBLOCKMINER));require(generated==2&&broadcasts==1,"Generator dispatch");
    std::cout<<"Miner worker: 16 real mined objects, cancellation/owner release, exception recovery, strict timeout and benchmark/routing adapters PASS\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
