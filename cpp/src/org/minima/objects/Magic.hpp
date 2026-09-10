#pragma once
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class Magic {
public:
    static const MiniNumber MIN_HASHES,DEFAULT_TXPOW_SIZE,DEFAULT_KISSVM_OPERATIONS,DEFAULT_TXPOW_TXNS;
    static const BigInt MIN_TXPOW_VAL;
    static const MiniData MIN_TXPOW_WORK;
    MiniNumber mCurrentMaxTxPoWSize{65536},mCurrentMaxKISSVMOps{1024},mCurrentMaxTxnPerBlock{256};
    MiniData mCurrentMinTxPoWWork;
    MiniNumber mDesiredMaxTxPoWSize{65536},mDesiredMaxKISSVMOps{1024},mDesiredMaxTxnPerBlock{256};
    MiniData mDesiredMinTxPoWWork;
    Magic();
    json::JSONObject toJSON() const;
    bool checkSame(const Magic& other) const;
    const MiniNumber& getMaxTxPoWSize() const {return mCurrentMaxTxPoWSize;}
    const MiniNumber& getMaxKISSOps() const {return mCurrentMaxKISSVMOps;}
    const MiniNumber& getMaxNumTxns() const {return mCurrentMaxTxnPerBlock;}
    const MiniData& getMinTxPowWork() const {return mCurrentMinTxPoWWork;}
    void setDesiredKISSVM(MiniNumber value) {mDesiredMaxKISSVMOps=std::move(value);}
    void setDesiredMaxTxPoWSize(MiniNumber value) {mDesiredMaxTxPoWSize=std::move(value);}
    void setDesiredMaxTxns(MiniNumber value) {mDesiredMaxTxnPerBlock=std::move(value);}
    Magic calculateNewCurrent() const;
    std::string toString() const {return toJSON().toJSONString();}
    std::vector<uint8_t> serialise() const;
    static Magic deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
