#include "RamDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <chrono>
#include <bit>
#include <algorithm>
namespace minima {
RamDB::RamDB():MAX_TIME(std::bit_cast<int64_t>(uint64_t(3600000)*uint64_t(GeneralParams::NUMBER_HOURS_RAMTXPOWDB))){}
bool RamDB::addTxPoW(std::shared_ptr<TxPoW> txpow) {
    if(!txpow)throw std::invalid_argument("Null TxPoW");auto current=getCompleteMemPool()->get(txpow->getTxPoWID());
    if(current){(*current)->updateLastAccess();return false;}
    auto id=txpow->getTxPoWID();getCompleteMemPool()->put(id,std::make_shared<RamData>(std::move(txpow)));return true;
}
std::shared_ptr<TxPoW> RamDB::getTxPoW(const std::string& id) {auto current=getCompleteMemPool()->get(id);if(!current)return nullptr;(*current)->updateLastAccess();return (*current)->getTxPoW();}
void RamDB::cleanDB() {cleanDBAt(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());}
void RamDB::cleanDBAt(int64_t now) {
    auto cutoff=std::bit_cast<int64_t>(uint64_t(now)-uint64_t(MAX_TIME));auto next=std::make_shared<Map>();
    for(const auto& data:getCompleteMemPool()->elements())if(data->getLastAccess()>cutoff)next->put(data->getTxPoW()->getTxPoWID(),data);
    std::lock_guard lock(mMapMutex);mTxPoWDB=next;
}
void RamDB::cleanUnusedTxn() {
    auto pool=getAllUnusedTxns();std::stable_sort(pool.begin(),pool.end(),[](const auto& a,const auto& b){return a->getBurn().isMore(b->getBurn());});
    int64_t count=0;for(const auto& txn:pool){if(count>MAX_UNUSED_TXNS)remove(txn->getTxPoWID());++count;}
}
void RamDB::clearMainChainTxns() {for(const auto& data:getCompleteMemPool()->elements())data->setOnMainChain(false);}
void RamDB::setOnMainChain(const std::string& id) {auto data=getCompleteMemPool()->get(id);if(data)(*data)->setOnMainChain(true);}
void RamDB::setInCascade(const std::string& id) {auto data=getCompleteMemPool()->get(id);if(data)(*data)->setInCascade(true);}
std::vector<std::shared_ptr<TxPoW>> RamDB::getAllUnusedTxns() const {
    std::vector<std::shared_ptr<TxPoW>> result;for(const auto& data:getCompleteMemPool()->elements())if(!data->isOnMainChain()&&data->getTxPoW()->isTransaction()&&!data->isInCascade())result.push_back(data->getTxPoW());return result;
}
bool RamDB::checkForCoinID(const MiniData& id) const {
    for(const auto& data:getCompleteMemPool()->elements())if(data->getTxPoW()->isTransaction()&&!data->isInCascade()){
        for(const Witness* wit:{&data->getTxPoW()->getWitness(),&data->getTxPoW()->getBurnWitness()})for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems)if(proof.getCoin().getCoinID().isEqual(id))return true;
    }
    return false;
}
}
