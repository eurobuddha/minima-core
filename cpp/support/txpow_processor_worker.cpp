#include "TxPoWProcessorFixture.hpp"
#include <fstream>
#include <future>
#include <latch>
#include <iostream>
using namespace minima;using namespace minima::processor_fixture;
namespace {
void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
auto readBlocks(const std::string& path){std::ifstream input(path);std::string line;std::getline(input,line);auto parsed=json::JSONValue::parseWithException(line);auto values=sql_fixture::object(parsed);std::vector<std::shared_ptr<TxBlock>> blocks;for(const auto& raw:sql_fixture::array(values.get("blocks")).elements()){auto data=MiniData::fromHex(raw.asString());size_t offset=0;blocks.push_back(std::make_shared<TxBlock>(TxBlock::deserialise(data.bytes(),offset)));}require(blocks.size()>=6,"Missing mined Java block fixture");return blocks;}
auto ibd(const std::vector<std::shared_ptr<TxBlock>>& blocks){auto result=std::make_shared<IBD>();result->getTxBlocks()->assign(blocks.begin(),blocks.begin()+6);return result;}
void flags(){shutdown=restore=relevant=sync=false;calls=0;failAt=-1;events=json::JSONArray();GeneralParams::USE_SQL_COINDB=false;GeneralParams::BLOCK_LOGS=false;GeneralParams::IBDSYNC_LOGS=false;GeneralParams::TXBLOCK_NODE=false;GeneralParams::TEST_PARAMS=false;GeneralParams::IS_MEGAMMR=false;GeneralParams::NO_SYNC_IBD=false;GlobalParams::MINIMA_CASCADE_START_DEPTH=MiniNumber(1000);GlobalParams::MINIMA_CASCADE_FREQUENCY=MiniNumber(1000);}
}
int main(int argc,char** argv)try{
    require(argc==2,"Pass mined fixture JSON");auto blocks=readBlocks(argv[1]);flags();auto db=std::make_shared<DB>();processor_fixture::bind(db);
    // Real owned worker: a malformed message must not prevent a following IBD,
    // and the worker must apply the inherited database-lock cleanup each time.
    std::latch done(18);std::atomic<int> cleaned=0;auto svc=services(db);svc.releaseLocks=[&]{db->safeReleaseReadWriteLock();++cleaned;done.count_down();};
    auto worker=TxPoWProcessor::create(svc);worker->postProcessIBD(nullptr,"0x00");worker->postProcessIBD(ibd(blocks),"0x00",true);for(int i=0;i<16;++i)worker->postCheckCall();done.wait();worker->stopMessageProcessor();worker->joinMessageProcessor();
    require(cleaned==18&&worker->isIBDProcessFinished()&&worker->isShutdownComplete(),"Processor recovery/finally/finish");require(db->getTxPoWTree()->getTip()->getSharedTxPoW()==blocks[5]->getSharedTxPoW(),"Worker lost block identity/tip");
    worker.reset();clearBindings();db.reset();
    // Stop and drop the outside processor owner while a callback is blocked.
    // Source processing already in progress continues, then releases its owner.
    flags();db=std::make_shared<DB>();processor_fixture::bind(db);std::promise<void> entered,resume;auto resuming=resume.get_future().share();std::latch ended(1);svc=services(db);auto setSync=svc.setSyncIBD;
    svc.setSyncIBD=[&](bool value){setSync(value);if(value){entered.set_value();resuming.wait();}};svc.releaseLocks=[&]{db->safeReleaseReadWriteLock();ended.count_down();};
    worker=TxPoWProcessor::create(svc);std::weak_ptr<TxPoWProcessor> retained=worker;worker->postProcessIBD(ibd(blocks),"0x00",true);entered.get_future().wait();worker->stopMessageProcessor();worker.reset();require(!retained.expired(),"Active processor lost ownership");resume.set_value();ended.wait();if(auto owner=retained.lock())owner->joinMessageProcessor();
    for(int i=0;i<100&&!retained.expired();++i)std::this_thread::sleep_for(std::chrono::milliseconds(2));require(retained.expired(),"Processor did not release after completion");require(db->getTxPoWTree()->getTip()->getSharedTxPoW()==blocks[5]->getSharedTxPoW(),"Stopped active IBD did not finish");clearBindings();db.reset();
    // Native explicit clock covers strict first-IBD age and recent-tip boundaries.
    for(bool test:{false,true})for(int64_t age:{int64_t(300000),int64_t(300001)})for(int direction:{-1,1})for(int delta:{-1,0,1}){
        flags();GeneralParams::TEST_PARAMS=test;db=std::make_shared<DB>();processor_fixture::bind(db);auto source=readBlocks(argv[1]);int64_t now=0;svc=services(db);svc.now=[&]{return now;};auto manual=std::make_shared<Processor>(svc);now=age;
        auto threshold=test?300000:10800000;source[0]->getTxPoW().setTimeMilli(MiniNumber(now+direction*(threshold+delta)));db->getTxPoWTree()->setRoot(std::make_shared<TxPoWTreeNode>(source[0]));auto input=std::make_shared<IBD>();input->getTxBlocks()->push_back(source[1]);manual->postProcessIBD(input,"0x00",true);manual->processMessage(manual->getNextMessage());
        bool rejected=age>300000&&delta<0;require((db->getTxPoWTree()->getTip()->getBlockNumber().getAsInt()==1)==rejected,"First-IBD/recent-tip strict clock boundary");require(manual->isIBDProcessFinished()!=rejected,"Early-return finish flag");require(!sync,"Sync flag not cleared on return");clearBindings();
    }
    clearBindings();std::cout<<"Processor worker recovery, FIFO cleanup, owner release and 24 strict clock cases PASS\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
