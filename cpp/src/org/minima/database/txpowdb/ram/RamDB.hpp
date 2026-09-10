#pragma once
#include "RamData.hpp"
#include "support/JavaConcurrentMap.hpp"
namespace minima {
class RamDB {
public:
    using Map=JavaConcurrentMap<std::shared_ptr<RamData>>;
private:
    mutable std::mutex mMapMutex;
    std::shared_ptr<Map> mTxPoWDB{std::make_shared<Map>()};
public:
    int64_t MAX_TIME;
    int32_t MAX_UNUSED_TXNS=500;
    RamDB();
    bool addTxPoW(std::shared_ptr<TxPoW> txpow);
    std::shared_ptr<Map> getCompleteMemPool() const {std::lock_guard lock(mMapMutex);return mTxPoWDB;}
    bool exists(const std::string& id) const {return getCompleteMemPool()->containsKey(id);}
    std::shared_ptr<TxPoW> getTxPoW(const std::string& id);
    void remove(const std::string& id) {getCompleteMemPool()->remove(id);}
    void cleanDB();
    void cleanDBAt(int64_t now);
    void cleanUnusedTxn();
    int getSize() const {return int(getCompleteMemPool()->size());}
    void wipeRamDB() {getCompleteMemPool()->clear();}
    void clearMainChainTxns();
    void setOnMainChain(const std::string& id);
    std::vector<std::shared_ptr<TxPoW>> getAllUnusedTxns() const;
    void setInCascade(const std::string& id);
    bool checkForCoinID(const MiniData& id) const;
};
}
