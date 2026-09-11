#pragma once
#include "org/minima/system/brains/TimedChecker.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "SqlRecordingFixture.hpp"
namespace minima::support {
// Same valid parent/generator setup as support/timed.cpp; real native checker.
inline json::JSONObject sharedCheckerFixture(const json::JSONObject& input){
    using namespace json;using sql_fixture::integer;
    GeneralParams::BLOCK_LOGS=false;GeneralParams::USE_SQL_COINDB=false;
    TxPoW parent;parent.setBlockNumber(MiniNumber(1));parent.setTimeMilli(MiniNumber(0));parent.setBlockDifficulty(Magic::MIN_TXPOW_WORK);for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i)parent.setSuperParent(i,MiniData::ZERO_TXPOWID);parent.setHeaderBodyHash();parent.calculateTXPOWID();
    auto tip=std::make_shared<TxPoWTreeNode>(parent);tip->getMMR()->addEntry(MMRData::CreateMMRDataLeafNode(MiniNumber(1),MiniNumber(1)));tip->getMMR()->finalizeSet();
    auto& svc=TxPoWGenerator::services;svc.getTip=[tip]{return tip;};svc.getRoot=[tip]{return tip;};svc.getHashRate=[]{return MiniNumber(1000000);};svc.getMagicDesiredKISSVM=[]{return Magic::DEFAULT_KISSVM_OPERATIONS;};svc.getMagicMaxTxPoWSize=[]{return Magic::DEFAULT_TXPOW_SIZE;};svc.getMagicMaxTxns=[]{return Magic::DEFAULT_TXPOW_TXNS;};svc.getAllUnusedTxns=[]{return std::vector<std::shared_ptr<TxPoW>>{};};
    struct Reset {~Reset(){TxPoWGenerator::services={};TxPoWChecker::services={};}} reset;
    TxPoWChecker::services.getCascadeTip=[]{return std::shared_ptr<CascadeNode>{};};
    auto block=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(Transaction{},Witness{}));auto list=std::make_shared<TxPoWChecker::TxPoWs>();
    auto txn=std::make_shared<TxPoW>();txn->setTxDifficulty(Magic::MIN_TXPOW_WORK);txn->calculateTXPOWID();for(int i=0;i<integer(input,"count",2);++i)list->push_back(txn);
    int mode=int(integer(input,"mode"));if(mode==1)list.reset();else if(mode==2)list->insert(list->begin(),nullptr);else if(mode==3)list->push_back(nullptr);else if(mode==4)block.reset();else if(mode==5)block->getTxHeader().mChainID=MiniData::fromHex("0x01");
    JSONArray steps;for(int i=0;i<3;++i){JSONObject row;bool valid;if(input.get("timed").asBool()){TimedChecker checker;valid=checker.checkTxPoWBlock(tip,block,list);row.put("finished",checker.isFinished());}else valid=TxPoWChecker::checkTxPoWBlock(tip,block,list);row.put("valid",valid);JSONArray flags;for(auto* t:{&txn->getTransaction(),&txn->getBurnTransaction()}){flags.add(t->mHaveCheckedMonotonic());flags.add(t->mIsMonotonic());flags.add(t->mIsValid());}row.put("flags",flags);steps.add(row);if(i==0)txn->getTransaction().mIsValid()=false;if(i==1)txn->getTransaction().clearIsMonotonic();}
    JSONObject out;out.put("steps",steps);return out;
}
}
