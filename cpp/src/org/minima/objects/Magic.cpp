#include "Magic.hpp"
#include <iostream>
namespace minima {
const std::shared_ptr<MiniNumber> Magic::MIN_HASHES=std::make_shared<MiniNumber>(10000),Magic::DEFAULT_TXPOW_SIZE=std::make_shared<MiniNumber>(65536),Magic::DEFAULT_KISSVM_OPERATIONS=std::make_shared<MiniNumber>(1024),Magic::DEFAULT_TXPOW_TXNS=std::make_shared<MiniNumber>(256);
// Keep initialization independent of another translation unit's dynamic globals.
const BigInt Magic::MIN_TXPOW_VAL=BigInt::fromMagnitudeBytes(std::vector<uint8_t>(32,255))/MIN_HASHES->getAsBigInteger();
const std::shared_ptr<MiniData> Magic::MIN_TXPOW_WORK=std::make_shared<MiniData>(MIN_TXPOW_VAL);
json::JSONObject Magic::toJSON() const {
    json::JSONObject obj;
    obj.put("currentmaxtxpowsize",required(mCurrentMaxTxPoWSize()).toString());
    obj.put("currentmaxkissvmops",required(mCurrentMaxKISSVMOps()).toString());
    obj.put("currentmaxtxn",required(mCurrentMaxTxnPerBlock()).toString());
    obj.put("currentmintxpowwork",required(mCurrentMinTxPoWWork()).to0xString());
    obj.put("desiredmaxtxpowsize",required(mDesiredMaxTxPoWSize()).toString());
    obj.put("desiredmaxkissvmops",required(mDesiredMaxKISSVMOps()).toString());
    obj.put("desiredmaxtxn",required(mDesiredMaxTxnPerBlock()).toString());
    obj.put("desiredmintxpowwork",required(mDesiredMinTxPoWWork()).to0xString());
    return obj;
}
bool Magic::checkSame(const Magic* other) const {
    if(!other)throw JavaNullPointerException("null Magic argument");
    const bool equal0=required(mCurrentMaxTxPoWSize()).isEqual(required(other->mCurrentMaxTxPoWSize()));
    const bool equal1=required(mCurrentMaxKISSVMOps()).isEqual(required(other->mCurrentMaxKISSVMOps()));
    const bool equal2=required(mCurrentMaxTxnPerBlock()).isEqual(required(other->mCurrentMaxTxnPerBlock()));
    const bool equal3=required(mCurrentMinTxPoWWork()).isEqual(required(other->mCurrentMinTxPoWWork()));
    return equal0&&equal1&&equal2&&equal3;
}
Magic Magic::calculateNewCurrent() const {
    Magic result;
    auto weighted=[](const std::shared_ptr<MiniNumber>& current,std::shared_ptr<MiniNumber> desired,int limit) {
        auto min=required(current).div(MiniNumber::TWO);
        auto max=required(current).mult(MiniNumber::TWO);
        if(required(desired).isLess(min))desired=std::make_shared<MiniNumber>(std::move(min));
        else if(required(desired).isMore(max))desired=std::make_shared<MiniNumber>(std::move(max));
        if(required(desired).isLess(MiniNumber(limit)))desired=std::make_shared<MiniNumber>(limit);
        return std::make_shared<MiniNumber>(required(current).mult(MiniNumber(16383)).add(required(desired)).div(MiniNumber(16384)));
    };
    result.mCurrentMaxTxPoWSize()=weighted(mCurrentMaxTxPoWSize(),mDesiredMaxTxPoWSize(),65536);
    result.mCurrentMaxKISSVMOps()=weighted(mCurrentMaxKISSVMOps(),mDesiredMaxKISSVMOps(),1024);
    result.mCurrentMaxTxnPerBlock()=weighted(mCurrentMaxTxnPerBlock(),mDesiredMaxTxnPerBlock(),256);
    auto old=required(mCurrentMinTxPoWWork()).getDataValue();
    auto min=old/BigInt(2),max=old*BigInt(2);
    auto desired=required(mDesiredMinTxPoWWork()).getDataValue();
    if(desired<min)desired=min;else if(desired>max)desired=max;
    if(desired>MIN_TXPOW_VAL)desired=MIN_TXPOW_VAL;
    result.mCurrentMinTxPoWWork()=std::make_shared<MiniData>((old*BigInt(16383)+desired)/BigInt(16384));return result;
}
void Magic::writeDataStream(std::vector<uint8_t>& out) const {
    required(mCurrentMaxTxPoWSize()).writeDataStream(out);
    required(mCurrentMaxKISSVMOps()).writeDataStream(out);
    required(mCurrentMaxTxnPerBlock()).writeDataStream(out);
    required(mCurrentMinTxPoWWork()).writeDataStream(out);
    required(mDesiredMaxTxPoWSize()).writeDataStream(out);
    required(mDesiredMaxKISSVMOps()).writeDataStream(out);
    required(mDesiredMaxTxnPerBlock()).writeDataStream(out);
    required(mDesiredMinTxPoWWork()).writeDataStream(out);
}
std::vector<uint8_t> Magic::serialise() const {return Streamable::serialise();}
void Magic::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mCurrentMaxTxPoWSize()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mCurrentMaxKISSVMOps()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mCurrentMaxTxnPerBlock()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mCurrentMinTxPoWWork()=std::make_shared<MiniData>(MiniData::deserialise(data,offset));
    mDesiredMaxTxPoWSize()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mDesiredMaxKISSVMOps()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mDesiredMaxTxnPerBlock()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));
    mDesiredMinTxPoWWork()=std::make_shared<MiniData>(MiniData::deserialise(data,offset));
}
Magic Magic::deserialise(std::span<const uint8_t> data,size_t& offset) {Magic magic;magic.readDataStream(data,offset);return magic;}
void Magic::main() {
    auto desired=std::make_shared<MiniNumber>(3000);
    std::cout<<"Start:1024 Desired:"<<desired->toString()<<'\n';
    int days=0;Magic current;
    for(int i=0;i<1728*50;++i){
        if(i%1000==0)current.mDesiredMaxKISSVMOps()=desired;
        current=current.calculateNewCurrent();
        if(i%50==0)std::cout<<++days<<") "<<required(current.mCurrentMaxKISSVMOps()).toString()<<'\n';
    }
}
}
