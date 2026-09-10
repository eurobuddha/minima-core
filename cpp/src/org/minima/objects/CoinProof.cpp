#include "CoinProof.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
MMRData CoinProof::getMMRData() const {return MMRData::CreateMMRDataLeafNode(mData->mCoin,mData->mCoin.getAmount());}
json::JSONObject CoinProof::toJSON() const {json::JSONObject obj;obj.put("coin",mData->mCoin.toJSON());obj.put("proof",mData->mProof.toJSON());return obj;}
std::optional<CoinProof> CoinProof::convertMiniDataVersion(const MiniData& data) {try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}}
std::vector<uint8_t> CoinProof::serialise() const {return wire::join(mData->mCoin,mData->mProof);}
void CoinProof::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->mCoin=Coin::deserialise(data,offset);
    mData->mProof=MMRProof::deserialise(data,offset);
}
CoinProof CoinProof::deserialise(std::span<const uint8_t> data,size_t& offset) {auto coin=Coin::deserialise(data,offset);auto proof=MMRProof::deserialise(data,offset);return CoinProof(std::move(coin),std::move(proof));}
}
