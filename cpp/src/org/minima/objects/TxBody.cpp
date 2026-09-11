#include "TxBody.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
json::JSONObject TxBody::toJSON() const {
    json::JSONObject obj;obj.put("prng",required(mPRNG()).to0xString());obj.put("txndiff",required(mTxnDifficulty()).to0xString());obj.put("txn",required(mTransaction()).toJSON());obj.put("witness",required(mWitness()).toJSON());obj.put("burntxn",required(mBurnTransaction()).toJSON());obj.put("burnwitness",required(mBurnWitness()).toJSON());
    json::JSONArray ids;for(const auto owner=mTxPowIDList();const auto id:required(owner).range())ids.add(required(id).to0xString());obj.put("txnlist",ids);return obj;
}
void TxBody::writeDataStream(std::vector<uint8_t>& out) const {
    wire::append(out,required(mPRNG()).serialiseHash());required(mTxnDifficulty()).writeDataStream(out);
    required(mTransaction()).writeDataStream(out);required(mWitness()).writeDataStream(out);
    required(mBurnTransaction()).writeDataStream(out);required(mBurnWitness()).writeDataStream(out);
    MiniNumber(int64_t(required(mTxPowIDList()).size())).writeDataStream(out);
    for(const auto owner=mTxPowIDList();const auto id:required(owner).range())wire::append(out,required(id).serialiseHash());
}
std::vector<uint8_t> TxBody::serialise() const {return Streamable::serialise();}
void TxBody::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mPRNG()=std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset));
    mTxnDifficulty()=std::make_shared<MiniData>(MiniData::deserialise(data,offset));
    {const auto child=mTransaction();required(child).readDataStream(data,offset);}
    {const auto child=mWitness();required(child).readDataStream(data,offset);}
    {const auto child=mBurnTransaction();required(child).readDataStream(data,offset);}
    {const auto child=mBurnWitness();required(child).readDataStream(data,offset);}
    mTxPowIDList()=std::make_shared<IDs>();int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i)mTxPowIDList()->add(std::make_shared<MiniData>(MiniData::deserialiseHash(data,offset)));
}
TxBody TxBody::deserialise(std::span<const uint8_t> data,size_t& offset) {TxBody body;body.readDataStream(data,offset);return body;}
}
