#include "org/minima/system/brains/TimedChecker.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include <future>
using namespace minima;using namespace minima::json;
namespace minima {struct TimedCheckerFixture {static bool valid(const TimedChecker& c){return c.mState->valid.load();}};}
struct Context {
    std::mutex lock;JSONArray events;std::vector<std::string> failures;size_t prints=0;bool armed=false,stalled=false;std::promise<void> releaseWorker;std::shared_future<void> release=releaseWorker.get_future().share();
    void record(const std::string& text){std::lock_guard guard(lock);events.add(text);}
    [[noreturn]] static void fail(const std::string& kind){if(kind=="error")throw JavaError("java.lang.LinkageError","logger");if(kind=="oom")throw JavaOutOfMemoryError("logger");if(kind=="io")throw wire::IOException("logger");throw JavaTypedException("java.lang.IllegalStateException","logger");}
};
struct Handler:NativeUncaughtExceptionHandler {
    std::shared_ptr<Context> state;explicit Handler(std::shared_ptr<Context> s):state(std::move(s)){}
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& e)override{state->record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(*e->toString()));}
};
static std::shared_ptr<TxPoW> readPow(const JSONObject& data,const std::string& key){auto bytes=MiniData::fromHex(data.get(key).asString()).bytes();size_t offset=0;return std::make_shared<TxPoW>(TxPoW::deserialise(bytes,offset));}
static void waitWorker(const std::shared_ptr<TxPoWChecker::TxPoWs>& witness){auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(witness.use_count()!=1){if(std::chrono::steady_clock::now()>deadline)throw std::runtime_error("worker owner timeout");std::this_thread::yield();}}
int main(int argc,char** argv)try{
    if(argc!=5)return 2;std::string mode=argv[1],mutation=argv[2],failureList=argv[3];bool blockLogs=std::string(argv[4])=="true",timed=mode!="block"&&mode!="only";auto state=std::make_shared<Context>();state->stalled=mode=="stalled";size_t start=0;for(;;){auto end=failureList.find(',',start);state->failures.push_back(failureList.substr(start,end-start));if(end==std::string::npos)break;start=end+1;}
    MinimaLogger::Runtime logger;logger.println=[state](const auto& full){std::string kind;bool delay=false;{std::lock_guard lock(state->lock);if(!state->armed)return;auto text=unicode::encodeUTF8(full);text=text.substr(text.find(" : ")+3);if(text.starts_with("     "))return;state->events.add("log:"+text);auto index=state->prints++;delay=state->stalled&&index==0;if(index<state->failures.size())kind=state->failures[index];}if(delay)state->release.wait();if(!kind.empty()&&kind!="none")Context::fail(kind);};MinimaLogger::setRuntime(logger);
    std::string line;std::getline(std::cin,line);auto parsed=parser::JSONParser().parse(line);const auto& data=*std::get<std::shared_ptr<JSONObject>>(parsed.value);
    GeneralParams::USE_SQL_COINDB=false;GeneralParams::BLOCK_LOGS=false;auto parent=readPow(data,"parent"),block=readPow(data,"block");auto tip=std::make_shared<TxPoWTreeNode>(*parent);tip->getMMR()->addEntry(MMRData::CreateMMRDataLeafNode(MiniNumber(1),MiniNumber(1)));tip->getMMR()->finalizeSet();TxPoWChecker::services.getCascadeTip=[]{return std::shared_ptr<CascadeNode>{};};auto transactions=std::make_shared<TxPoWChecker::TxPoWs>();TimedChecker checker;
    if(timed&&mode!="fresh"){bool prime=mutation=="valid"||mode=="primed"||state->stalled;auto chain=block->getChainID();if(!prime)block->getTxHeader().mChainID=MiniData::fromHex("0xFE");if(checker.checkTxPoWBlock(tip,block,transactions)!=prime)throw std::runtime_error("unexpected prime result");waitWorker(transactions);if(!checker.isFinished())throw std::runtime_error("prime not finished");block->getTxHeader().mChainID=chain;}
    if(mutation=="chain")block->getTxHeader().mChainID=MiniData::fromHex("0xFF");else if(mutation=="height")block->setBlockNumber(MiniNumber(99));else if(mutation=="parents")block->setSuperParent(0,MiniData::fromHex("0xFF"));else if(mutation=="time-before")block->setTimeMilli(MiniNumber(-1));else if(mutation=="time-after")block->setTimeMilli(MiniNumber(86400001));else if(mutation=="difficulty")block->setBlockDifficulty(MiniData::fromHex("0x01"));else if(mutation=="magic")block->getMagic().mCurrentMaxTxnPerBlock()=std::make_shared<MiniNumber>(MiniNumber(7));else if(mutation=="count"){for(int i=0;i<=Magic::required(block->getMagic().getMaxNumTxns()).getAsInt();++i)block->addBlockTxPOW(MiniData::ZERO_TXPOWID);}else if(mutation=="empty-mmr")*tip->getMMR()=MMR();else if(mutation=="root")block->setMMRRoot(MiniData::fromHex("0xFF"));else if(mutation=="duplicate"||mutation=="duplicate-null"){transactions->push_back(readPow(data,"duplicate"));if(mutation=="duplicate-null")transactions->push_back(nullptr);}else if(mutation=="null-block")block.reset();else if(mutation=="null-parent")tip.reset();else if(mutation=="null-transactions")transactions.reset();else if(mutation=="null-transaction")transactions->push_back(nullptr);
    // Observe the worker's retained block when the transaction list is null;
    // otherwise its original shared transaction list provides the owner barrier.
    {std::lock_guard lock(state->lock);state->armed=true;}GeneralParams::BLOCK_LOGS=blockLogs;setDefaultUncaughtExceptionHandler(std::make_shared<Handler>(state));auto started=std::chrono::steady_clock::now();JSONValue returned;
    try{returned=timed?checker.checkTxPoWBlock(tip,block,transactions):mode=="only"?TxPoWChecker::checkTxBlockOnly(tip,TxBlock(*block)):TxPoWChecker::checkTxPoWBlock(tip,block,transactions);}catch(const std::exception& e){returned="escaped:"+nativeExceptionText(e);}
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-started).count();if(state->stalled)state->releaseWorker.set_value();
    if(timed){if(transactions)waitWorker(transactions);else{auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(block.use_count()!=1){if(std::chrono::steady_clock::now()>deadline)throw std::runtime_error("block owner timeout");std::this_thread::yield();}}}
    JSONObject out;{std::lock_guard lock(state->lock);out.put("events",state->events);}out.put("returned",returned);if(timed){out.put("finished",checker.isFinished());out.put("valid",TimedCheckerFixture::valid(checker));out.put("deadline",elapsed>=TimedChecker::MAX_CHECKTIME-1000);}std::cout<<out.toJSONString()<<'\n';setDefaultUncaughtExceptionHandler(nullptr);MinimaLogger::setRuntime({});TxPoWChecker::services={};
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
