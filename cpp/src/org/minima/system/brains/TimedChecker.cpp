#include "TimedChecker.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <chrono>
#include <thread>
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
bool TimedChecker::checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<std::vector<TxPoW>> transactions) {
    std::shared_ptr<TxPoWChecker::TxPoWs> aliases;
    if(transactions){
        struct OwnedValues:TxPoWChecker::TxPoWs {std::shared_ptr<std::vector<TxPoW>> values;};
        auto list=std::make_shared<OwnedValues>();list->values=transactions;
        list->reserve(transactions->size());for(auto& txn:*transactions)list->emplace_back(transactions,&txn);
        aliases=std::move(list); // Retain the source list even when it is empty.
    }
    return checkTxPoWBlock(std::move(parent),std::move(txpow),std::move(aliases));
}
bool TimedChecker::checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<TxPoWChecker::TxPoWs> transactions) {
    using Clock=std::chrono::system_clock;
    const auto start=Clock::now();
    auto elapsed=[&]{return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now()-start).count();};
    try {
        // Tree parents are weak native references. Preserve the complete ancestry
        // during the worker's lifetime, even if the chain is pruned concurrently.
        std::vector<std::shared_ptr<TxPoWTreeNode>> ancestors;
        for(auto node=parent;node;node=node->getParent()) ancestors.push_back(node);
        auto thread=std::make_shared<NativeThread>(NativeThread{nextNativeThreadName()});
        {std::lock_guard lock(mThreadLock);mCheckerThread=thread;}
        std::thread worker([state=mState,parent,txpow,transactions,ancestors=std::move(ancestors),thread]()mutable {
            thread->id=std::this_thread::get_id();
            runNativeThread(*thread,[&]{
                try {state->valid=TxPoWChecker::checkTxPoWBlock(parent,txpow,transactions);}
                catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
                catch(const std::exception& e) {MinimaLogger::log("Block failed to process : "+nativeExceptionText(e));state->valid=false;}
                state->finished=true;
            });
        });
        // Interruption is cooperative. Retain every input until the worker exits,
        // including when the polling caller returns before validation completes.
        worker.detach();
        int64_t duration=0;
        while(duration<MAX_CHECKTIME) {
            nativeThreadSleep(50);
            if(mState->finished.load()) break;
            duration=elapsed();
        }
        // Like the source field, target the current worker on this checker.
        // An interrupted polling sleep skips this statement and enters the catch.
        {std::lock_guard lock(mThreadLock);if(mCheckerThread)mCheckerThread->interrupt();}
    } catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& e) {MinimaLogger::log(std::make_shared<NativeThrowable>(e));mState->valid=false;}
    if(GeneralParams::BLOCK_LOGS){
        const auto duration=elapsed();
        if(!txpow)throw JavaNullPointerException("null block TxPoW");
        MinimaLogger::log(std::string("[VALID:")+(mState->valid.load()?"true":"false")+"] Block checker time : "+std::to_string(duration)+"ms @ "+txpow->getBlockNumber().toString()+" "+txpow->getTxPoWID());
    }
    return mState->valid.load();
}
bool TxPoWChecker::checkTxPoWBlockTimed(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<std::vector<TxPoW>> transactions) {
    TimedChecker checker;return checker.checkTxPoWBlock(std::move(parent),std::move(txpow),std::move(transactions));
}
bool TxPoWChecker::checkTxPoWBlockTimed(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<TxPoWs> transactions) {
    TimedChecker checker;return checker.checkTxPoWBlock(std::move(parent),std::move(txpow),std::move(transactions));
}
}
