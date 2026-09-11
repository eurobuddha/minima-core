#pragma once
#include "TxPoWChecker.hpp"
#include <atomic>
#include "support/NativeThread.hpp"
namespace minima {
class TimedChecker {
    friend struct TimedCheckerFixture;
    struct State {std::atomic<bool> finished{false},valid{false};};
    std::shared_ptr<State> mState=std::make_shared<State>();
    std::mutex mThreadLock;
    std::shared_ptr<NativeThread> mCheckerThread;
public:
    static constexpr int64_t MAX_CHECKTIME=120000;
    // Callers must not mutate validation inputs until the worker finishes,
    // including after a timeout. Shared ownership keeps asynchronous inputs alive.
    bool checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<std::vector<TxPoW>> transactions);
    bool checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<TxPoWChecker::TxPoWs> transactions);
    bool isFinished() const {return mState->finished.load();}
};
}
