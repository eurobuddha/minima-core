#include "org/minima/system/brains/TimedChecker.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include <chrono>
#include <thread>
#include <iostream>
using namespace minima;
static void require(bool value){if(!value)throw std::runtime_error("Timed checker assertion");}
int main(int argc,char** argv){
    using Clock=std::chrono::steady_clock;
    TxPoW parent;parent.setBlockNumber(MiniNumber(1));parent.setTimeMilli(MiniNumber(0));parent.setBlockDifficulty((*Magic::MIN_TXPOW_WORK));for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i)parent.setSuperParent(i,MiniData::ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();
    auto tip=std::make_shared<TxPoWTreeNode>(parent);tip->getMMR()->addEntry(MMRData::CreateMMRDataLeafNode(MiniNumber(1),MiniNumber(1)));tip->getMMR()->finalizeSet();
    auto& svc=TxPoWGenerator::services;svc.getTip=[tip]{return tip;};svc.getRoot=[tip]{return tip;};svc.getHashRate=[]{return MiniNumber(1000000);};svc.getMagicDesiredKISSVM=[]{return (*Magic::DEFAULT_KISSVM_OPERATIONS);};svc.getMagicMaxTxPoWSize=[]{return (*Magic::DEFAULT_TXPOW_SIZE);};svc.getMagicMaxTxns=[]{return (*Magic::DEFAULT_TXPOW_TXNS);};svc.getAllUnusedTxns=[]{return std::vector<std::shared_ptr<TxPoW>>{};};
    auto block=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(Transaction{},Witness{}));auto txns=std::make_shared<std::vector<TxPoW>>();
    TxPoWChecker::services.getCascadeTip=[]{return std::shared_ptr<CascadeNode>{};};
    if(argc==1){
        TimedChecker valid;require(valid.checkTxPoWBlock(tip,block,txns));require(valid.isFinished());
        block->getTxHeader().mChainID()=std::make_shared<MiniData>(MiniData::fromHex("0xFF"));TimedChecker invalid;require(!invalid.checkTxPoWBlock(tip,block,txns));require(invalid.isFinished());
        std::cout<<"Timed checker normal valid/invalid cases passed\n";
    }else{
        bool shared=std::string(argv[1])=="--shared-deadline";
        auto sharedTxns=std::make_shared<TxPoWChecker::TxPoWs>();
        auto transaction=std::make_shared<TxPoW>();transaction->setTxDifficulty((*Magic::MIN_TXPOW_WORK));
        sharedTxns->push_back(transaction);
        auto entered=std::make_shared<std::atomic<bool>>(false);
        TxPoWChecker::services.getCascadeTip=[entered]{*entered=true;std::this_thread::sleep_for(std::chrono::milliseconds(TimedChecker::MAX_CHECKTIME+500));return std::shared_ptr<CascadeNode>{};};
        TimedChecker checker;const auto start=Clock::now();require(!(shared?checker.checkTxPoWBlock(tip,block,sharedTxns):checker.checkTxPoWBlock(tip,block,txns)));require(entered->load());require(!checker.isFinished());
        auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-start).count();require(elapsed>=TimedChecker::MAX_CHECKTIME-1000&&elapsed<TimedChecker::MAX_CHECKTIME+10000);
        std::weak_ptr<TxPoW> retainedBlock=block,retainedTransaction=transaction;
        std::weak_ptr<std::vector<TxPoW>> retainedTxns=txns;
        std::weak_ptr<TxPoWChecker::TxPoWs> retainedShared=sharedTxns;
        block.reset();txns.reset();sharedTxns.reset();transaction.reset();
        require(!retainedBlock.expired());
        if(shared)require(!retainedShared.expired()&&!retainedTransaction.expired());else require(!retainedTxns.expired());
        auto finishDeadline=Clock::now()+std::chrono::seconds(10);while(!checker.isFinished()&&Clock::now()<finishDeadline)std::this_thread::sleep_for(std::chrono::milliseconds(10));require(checker.isFinished());
        while((!retainedBlock.expired()||!retainedTxns.expired()||!retainedShared.expired()||!retainedTransaction.expired())&&Clock::now()<finishDeadline)std::this_thread::sleep_for(std::chrono::milliseconds(10));
        require(retainedBlock.expired()&&retainedTxns.expired()&&retainedShared.expired()&&retainedTransaction.expired());
        std::cout<<"120-second deadline and asynchronous ownership passed\n";
    }
    TxPoWGenerator::services={};TxPoWChecker::services={};
}
