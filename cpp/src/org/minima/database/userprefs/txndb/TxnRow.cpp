#include "TxnRow.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
json::JSONObject TxnRow::toJSON(bool showWitness){
    json::JSONObject ret;ret.put("id",mID?json::JSONValue(*mID):json::JSONValue());
    if(!mTransaction)throw std::invalid_argument("Null transaction");TxPoWGenerator::precomputeTransactionCoinID(*mTransaction);ret.put("transaction",mTransaction->toJSON());
    if(showWitness){if(!mWitness)throw std::invalid_argument("Null witness");ret.put("witness",mWitness->toJSON());json::JSONArray coins;for(const auto txnItems=mTransaction->getAllOutputs();const auto& coin:*txnItems)coins.add(MiniData(coin.serialise()).to0xString());ret.put("outputcoindata",coins);}return ret;
}
std::vector<uint8_t> TxnRow::serialise() const {if(!mTransaction||!mWitness)throw std::invalid_argument("Null transaction/witness");return wire::join(MiniString(mID.value()),*mTransaction,*mWitness);}
void TxnRow::readDataStream(std::span<const uint8_t> data,size_t& offset){mID=MiniString::deserialise(data,offset).toString();mTransaction=std::make_shared<Transaction>();mTransaction->readDataStream(data,offset);mWitness=std::make_shared<Witness>();mWitness->readDataStream(data,offset);}
std::shared_ptr<TxnRow> TxnRow::deserialise(std::span<const uint8_t> data,size_t& offset){auto row=std::shared_ptr<TxnRow>(new TxnRow);row->readDataStream(data,offset);return row;}
std::shared_ptr<TxnRow> TxnRow::convertMiniDataVersion(const MiniData& data){try{size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& e){MinimaLogger::log(std::make_shared<NativeThrowable>(e));return nullptr;}}
}
