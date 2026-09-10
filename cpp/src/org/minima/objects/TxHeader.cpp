#include "TxHeader.hpp"
#include "base/MiniByte.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <chrono>
#include "support/JavaDateString.hpp"
namespace minima {
MiniData TxHeader::MAIN_NET=MiniData::fromHex("0x00"),TxHeader::TEST_NET=MiniData::fromHex("0x01");
TxHeader::TxHeader():mChainID(MAIN_NET),mTimeMilli(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {
    if(GlobalParams::MINIMA_CASCADE_LEVELS<0) throw std::invalid_argument("Negative cascade levels");mSuperParents.resize(size_t(GlobalParams::MINIMA_CASCADE_LEVELS));
}
json::JSONObject TxHeader::toJSON() const {
    json::JSONObject obj;obj.put("chainid",mChainID.to0xString());obj.put("block",mBlockNumber.toString());obj.put("blkdiff",mBlockDifficulty.to0xString());obj.put("cascadelevels",GlobalParams::MINIMA_CASCADE_LEVELS);
    json::JSONArray supers;int count=0;const MiniData* previous=nullptr;
    auto add=[&](int difficulty,const MiniData& parent) {json::JSONObject sp;sp.put("difficulty",difficulty);sp.put("count",count);sp.put("parent",parent.to0xString());supers.add(sp);};
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) {
        const auto& current=mSuperParents.at(size_t(i));if(!previous) {previous=&current;++count;}else if(previous->isEqual(current)) ++count;else {add(i-1,*previous);previous=&current;count=1;}
        if(i==GlobalParams::MINIMA_CASCADE_LEVELS-1) add(i,current);
    }
    obj.put("superparents",supers);obj.put("magic",mMagic.toJSON());obj.put("mmr",mMMRRoot.to0xString());obj.put("total",mMMRTotal.toString());obj.put("customhash",mCustomHash.to0xString());obj.put("txbodyhash",mTxBodyHash.to0xString());obj.put("nonce",mNonce.toString());obj.put("timemilli",mTimeMilli.toString());obj.put("date",javaDateString(mTimeMilli.getAsLong()));return obj;
}
std::vector<uint8_t> TxHeader::serialise() const {
    auto out=wire::join(mNonce,mChainID,mTimeMilli,mBlockNumber,mBlockDifficulty);int count=0;const MiniData* previous=nullptr;
    auto add=[&]() {wire::append(out,MiniByte(count).serialise());wire::append(out,previous->serialiseHash());};
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) {
        const auto& current=mSuperParents.at(size_t(i));if(!previous) {previous=&current;++count;}else if(previous->isEqual(current)) ++count;else {add();previous=&current;count=1;}
        if(i==GlobalParams::MINIMA_CASCADE_LEVELS-1) add();
    }
    wire::append(out,mMMRRoot.serialiseHash());wire::append(out,wire::join(mMMRTotal,mMagic));wire::append(out,mCustomHash.serialiseHash());wire::append(out,mTxBodyHash.serialiseHash());return out;
}
TxHeader TxHeader::deserialise(std::span<const uint8_t> data,size_t& offset) {
    TxHeader header;header.mNonce=MiniNumber::deserialise(data,offset);header.mChainID=MiniData::deserialise(data,offset);header.mTimeMilli=MiniNumber::deserialise(data,offset);header.mBlockNumber=MiniNumber::deserialise(data,offset);header.mBlockDifficulty=MiniData::deserialise(data,offset);
    size_t filled=0;while(filled<header.mSuperParents.size()) {int count=MiniByte::deserialise(data,offset).getValue();auto parent=MiniData::deserialiseHash(data,offset);if(size_t(count)>header.mSuperParents.size()-filled) throw std::out_of_range("Superparent run exceeds cascade levels");for(int i=0;i<count;++i) header.mSuperParents[filled++]=parent;}
    header.mMMRRoot=MiniData::deserialiseHash(data,offset);header.mMMRTotal=MiniNumber::deserialise(data,offset);header.mMagic=Magic::deserialise(data,offset);header.mCustomHash=MiniData::deserialiseHash(data,offset);header.mTxBodyHash=MiniData::deserialiseHash(data,offset);return header;
}
std::optional<TxHeader> TxHeader::convertMiniDataVersion(const MiniData& data) {try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const std::exception&) {return std::nullopt;}}
}
