#pragma once
#include "org/minima/objects/TxPoW.hpp"
namespace minima {
struct RelayPolicy {
    static bool checkMaxCoinOutputNumber(const TxPoW& txpow);
    static bool checkMaxStateStoreSize(const TxPoW& txpow,int64_t maximum);
    static bool checkAllPolicies(const TxPoW& txpow,int64_t maximum);
};
}
