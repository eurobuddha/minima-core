#pragma once
#include "org/minima/objects/TxPoW.hpp"
#include <atomic>
namespace minima {
class RamData {
    std::shared_ptr<TxPoW> mTxPoW;
    std::atomic<int64_t> mLastAccess;
    std::atomic<bool> mIsOnMainChain{false},mIsInCascade{false};
public:
    explicit RamData(std::shared_ptr<TxPoW> txpow);
    std::shared_ptr<TxPoW> getTxPoW() const {return mTxPoW;}
    void updateLastAccess();
    int64_t getLastAccess() const {return mLastAccess.load();}
    void setOnMainChain(bool onChain) {mIsOnMainChain=onChain;}
    bool isOnMainChain() const {return mIsOnMainChain.load();}
    void setInCascade(bool cascade) {mIsInCascade=cascade;}
    bool isInCascade() const {return mIsInCascade.load();}
};
}
