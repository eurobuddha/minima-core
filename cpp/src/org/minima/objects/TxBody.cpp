#include "TxBody.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
namespace {
template<class T> T& required(const std::shared_ptr<T>& value) {
    if(!value) throw JavaNullPointerException("null TxBody field");return *value;
}
}
json::JSONObject TxBody::toJSON() const {
    json::JSONObject obj;obj.put("prng",mPRNG.to0xString());obj.put("txndiff",mTxnDifficulty.to0xString());obj.put("txn",required(mTransaction).toJSON());obj.put("witness",required(mWitness).toJSON());obj.put("burntxn",required(mBurnTransaction).toJSON());obj.put("burnwitness",required(mBurnWitness).toJSON());
    json::JSONArray ids;for(const auto& id:mTxPowIDList) ids.add(id.to0xString());obj.put("txnlist",ids);return obj;
}
std::vector<uint8_t> TxBody::serialise() const {
    auto out=mPRNG.serialiseHash();wire::append(out,mTxnDifficulty.serialise());
    wire::append(out,required(mTransaction).serialise());wire::append(out,required(mWitness).serialise());
    wire::append(out,required(mBurnTransaction).serialise());wire::append(out,required(mBurnWitness).serialise());
    wire::append(out,MiniNumber(int64_t(mTxPowIDList.size())).serialise());
    for(const auto& id:mTxPowIDList) wire::append(out,id.serialiseHash());return out;
}
void TxBody::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mPRNG=MiniData::deserialiseHash(data,offset);mTxnDifficulty=MiniData::deserialise(data,offset);
    required(mTransaction).readDataStream(data,offset);required(mWitness).readDataStream(data,offset);
    required(mBurnTransaction).readDataStream(data,offset);required(mBurnWitness).readDataStream(data,offset);
    mTxPowIDList.clear();int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i) mTxPowIDList.push_back(MiniData::deserialiseHash(data,offset));
}
TxBody TxBody::deserialise(std::span<const uint8_t> data,size_t& offset) {TxBody body;body.readDataStream(data,offset);return body;}
}
