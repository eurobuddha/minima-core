#include "Magic.hpp"
namespace minima {
const MiniNumber Magic::MIN_HASHES(10000),Magic::DEFAULT_TXPOW_SIZE(65536),Magic::DEFAULT_KISSVM_OPERATIONS(1024),Magic::DEFAULT_TXPOW_TXNS(256);
const BigInt Magic::MIN_TXPOW_VAL=BigInt::fromMagnitudeBytes(std::vector<uint8_t>(32,255))/BigInt(10000);
const MiniData Magic::MIN_TXPOW_WORK(Magic::MIN_TXPOW_VAL);
Magic::Magic():mCurrentMinTxPoWWork(MIN_TXPOW_WORK),mDesiredMinTxPoWWork(MIN_TXPOW_WORK){}
json::JSONObject Magic::toJSON() const {
    json::JSONObject obj;obj.put("currentmaxtxpowsize",mCurrentMaxTxPoWSize.toString());obj.put("currentmaxkissvmops",mCurrentMaxKISSVMOps.toString());obj.put("currentmaxtxn",mCurrentMaxTxnPerBlock.toString());obj.put("currentmintxpowwork",mCurrentMinTxPoWWork.to0xString());
    obj.put("desiredmaxtxpowsize",mDesiredMaxTxPoWSize.toString());obj.put("desiredmaxkissvmops",mDesiredMaxKISSVMOps.toString());obj.put("desiredmaxtxn",mDesiredMaxTxnPerBlock.toString());obj.put("desiredmintxpowwork",mDesiredMinTxPoWWork.to0xString());return obj;
}
bool Magic::checkSame(const Magic& other) const {return mCurrentMaxTxPoWSize.isEqual(other.mCurrentMaxTxPoWSize)&&mCurrentMaxKISSVMOps.isEqual(other.mCurrentMaxKISSVMOps)&&mCurrentMaxTxnPerBlock.isEqual(other.mCurrentMaxTxnPerBlock)&&mCurrentMinTxPoWWork.isEqual(other.mCurrentMinTxPoWWork);}
Magic Magic::calculateNewCurrent() const {
    auto weighted=[](const MiniNumber& current,MiniNumber desired,int limit) {
        auto min=current.div(MiniNumber(2)),max=current.mult(MiniNumber(2));if(desired.isLess(min)) desired=min;else if(desired.isMore(max)) desired=max;
        if(desired.isLess(MiniNumber(limit))) desired=MiniNumber(limit);
        return current.mult(MiniNumber(16383)).add(desired).div(MiniNumber(16384));
    };
    Magic result;result.mCurrentMaxTxPoWSize=weighted(mCurrentMaxTxPoWSize,mDesiredMaxTxPoWSize,65536);
    result.mCurrentMaxKISSVMOps=weighted(mCurrentMaxKISSVMOps,mDesiredMaxKISSVMOps,1024);
    result.mCurrentMaxTxnPerBlock=weighted(mCurrentMaxTxnPerBlock,mDesiredMaxTxnPerBlock,256);
    auto old=mCurrentMinTxPoWWork.getDataValue(),desired=mDesiredMinTxPoWWork.getDataValue();auto min=old/BigInt(2),max=old*BigInt(2);
    if(desired<min) desired=min;else if(desired>max) desired=max;if(desired>MIN_TXPOW_VAL) desired=MIN_TXPOW_VAL;
    result.mCurrentMinTxPoWWork=MiniData((old*BigInt(16383)+desired)/BigInt(16384));return result;
}
std::vector<uint8_t> Magic::serialise() const {return wire::join(mCurrentMaxTxPoWSize,mCurrentMaxKISSVMOps,mCurrentMaxTxnPerBlock,mCurrentMinTxPoWWork,mDesiredMaxTxPoWSize,mDesiredMaxKISSVMOps,mDesiredMaxTxnPerBlock,mDesiredMinTxPoWWork);}
Magic Magic::deserialise(std::span<const uint8_t> data,size_t& offset) {
    Magic magic;magic.mCurrentMaxTxPoWSize=MiniNumber::deserialise(data,offset);magic.mCurrentMaxKISSVMOps=MiniNumber::deserialise(data,offset);magic.mCurrentMaxTxnPerBlock=MiniNumber::deserialise(data,offset);magic.mCurrentMinTxPoWWork=MiniData::deserialise(data,offset);
    magic.mDesiredMaxTxPoWSize=MiniNumber::deserialise(data,offset);magic.mDesiredMaxKISSVMOps=MiniNumber::deserialise(data,offset);magic.mDesiredMaxTxnPerBlock=MiniNumber::deserialise(data,offset);magic.mDesiredMinTxPoWWork=MiniData::deserialise(data,offset);return magic;
}
}
