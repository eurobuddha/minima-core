#include "TxHeader.hpp"
#include "base/MiniByte.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaDateString.hpp"
#include "support/NativeThrowable.hpp"
#include <chrono>
namespace minima {
std::shared_ptr<MiniData> TxHeader::MAIN_NET=std::make_shared<MiniData>(MiniData::fromHex("0x00"));
std::shared_ptr<MiniData> TxHeader::TEST_NET=std::make_shared<MiniData>(MiniData::fromHex("0x01"));
TxHeader::TxHeader() {
    mNonce()=std::make_shared<MiniNumber>(0);mChainID()=MAIN_NET;
    mTimeMilli()=std::make_shared<MiniNumber>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    mBlockNumber()=std::make_shared<MiniNumber>(0);mBlockDifficulty()=Crypto::sharedMaxHash();
    mMagic()=std::make_shared<Magic>();mMMRRoot()=std::make_shared<MiniData>(MiniData::fromHex("0x00"));
    mMMRTotal()=MiniNumber::sharedZero();mCustomHash()=MiniData::sharedZeroTxPoWID();mTxBodyHash()=std::make_shared<MiniData>(MiniData::fromHex("0x00"));
    if(GlobalParams::MINIMA_CASCADE_LEVELS<0)throw JavaNegativeArraySizeException(std::to_string(GlobalParams::MINIMA_CASCADE_LEVELS));
    mSuperParents()=std::make_shared<Parents>(size_t(GlobalParams::MINIMA_CASCADE_LEVELS));
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i)parentAt(mSuperParents(),i)=std::make_shared<MiniData>();
}
json::JSONObject TxHeader::toJSON() const {
    json::JSONObject obj;obj.put("chainid",required(mChainID()).to0xString());obj.put("block",required(mBlockNumber()).toString());obj.put("blkdiff",required(mBlockDifficulty()).to0xString());obj.put("cascadelevels",GlobalParams::MINIMA_CASCADE_LEVELS);
    json::JSONArray supers;int count=0;std::shared_ptr<MiniData> previous;
    auto add=[&](int difficulty,const std::shared_ptr<MiniData>& parent){json::JSONObject sp;sp.put("difficulty",difficulty);sp.put("count",count);sp.put("parent",required(parent).to0xString());supers.add(sp);};
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i){
        auto current=parentAt(mSuperParents(),i);
        if(!previous){previous=current;++count;}else if(previous->isEqual(required(current)))++count;else{add(i-1,previous);previous=current;count=1;}
        if(i==GlobalParams::MINIMA_CASCADE_LEVELS-1)add(i,current);
    }
    obj.put("superparents",supers);obj.put("magic",required(mMagic()).toJSON());obj.put("mmr",required(mMMRRoot()).to0xString());obj.put("total",required(mMMRTotal()).toString());obj.put("customhash",required(mCustomHash()).to0xString());obj.put("txbodyhash",required(mTxBodyHash()).to0xString());obj.put("nonce",required(mNonce()).toString());obj.put("timemilli",required(mTimeMilli()).toString());obj.put("date",javaDateString(required(mTimeMilli()).getAsLong()));return obj;
}
void TxHeader::writeDataStream(std::vector<uint8_t>& out) const {
    required(mNonce()).writeDataStream(out);required(mChainID()).writeDataStream(out);required(mTimeMilli()).writeDataStream(out);required(mBlockNumber()).writeDataStream(out);required(mBlockDifficulty()).writeDataStream(out);
    int count=0;std::shared_ptr<MiniData> previous;
    auto add=[&]{MiniByte(count).writeDataStream(out);required(previous).writeHashToStream(out);};
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i){
        auto current=parentAt(mSuperParents(),i);
        if(!previous){previous=current;++count;}else if(previous->isEqual(required(current)))++count;else{add();previous=current;count=1;}
        if(i==GlobalParams::MINIMA_CASCADE_LEVELS-1)add();
    }
    required(mMMRRoot()).writeHashToStream(out);required(mMMRTotal()).writeDataStream(out);required(mMagic()).writeDataStream(out);required(mCustomHash()).writeHashToStream(out);required(mTxBodyHash()).writeHashToStream(out);
}
std::vector<uint8_t> TxHeader::serialise() const {return Streamable::serialise();}
void TxHeader::readDataStream(std::span<const uint8_t> data,size_t& offset){
    mNonce()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));mChainID()=std::make_shared<MiniData>(MiniData::deserialise(data,offset));mTimeMilli()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));mBlockNumber()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));mBlockDifficulty()=std::make_shared<MiniData>(MiniData::deserialise(data,offset));
    int filled=0;
    while(filled<GlobalParams::MINIMA_CASCADE_LEVELS){
        auto len=MiniByte::deserialise(data,offset);auto parent=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));int count=len.getValue();
        for(int i=0;i<count;++i)parentAt(mSuperParents(),filled++)=parent;
    }
    mMMRRoot()=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));mMMRTotal()=std::make_shared<MiniNumber>(MiniNumber::deserialise(data,offset));mMagic()=std::make_shared<Magic>(Magic::deserialise(data,offset));mCustomHash()=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));mTxBodyHash()=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));
}
TxHeader TxHeader::deserialise(std::span<const uint8_t> data,size_t& offset){TxHeader header;header.readDataStream(data,offset);return header;}
std::optional<TxHeader> TxHeader::convertMiniDataVersion(const MiniData& data){return convertMiniDataVersion(&data);}
std::optional<TxHeader> TxHeader::convertMiniDataVersion(const MiniData* data){
    if(!data)throw JavaNullPointerException("null TxHeader input");auto bytes=data->bytes();
    try{size_t offset=0;return deserialise(bytes,offset);}catch(const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}
}
}
