#include "RelayPolicy.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <bit>
namespace minima {
bool RelayPolicy::checkMaxCoinOutputNumber(const TxPoW& txpow) {
    return int64_t(txpow.getTransaction().getAllOutputs().size())<=GeneralParams::MAX_RELAY_OUTPUTCOINS &&
           int64_t(txpow.getBurnTransaction().getAllOutputs().size())<=GeneralParams::MAX_RELAY_OUTPUTCOINS;
}
bool RelayPolicy::checkMaxStateStoreSize(const TxPoW& txpow,int64_t maximum) {
    auto size=[](const Transaction& txn) {
        uint32_t count=0;for(const auto& coin:txn.getAllOutputs()) if(coin.storeState()) ++count;
        return uint64_t(txn.calculateStateSize())*uint64_t(int64_t(std::bit_cast<int32_t>(count)));
    };
    return std::bit_cast<int64_t>(size(txpow.getTransaction())+size(txpow.getBurnTransaction()))<=maximum;
}
bool RelayPolicy::checkAllPolicies(const TxPoW& txpow,int64_t maximum) {
    return checkMaxCoinOutputNumber(txpow)&&checkMaxStateStoreSize(txpow,maximum);
}
}
