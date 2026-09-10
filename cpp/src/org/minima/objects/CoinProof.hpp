#pragma once
#include "Coin.hpp"
#include "mmr/MMRProof.hpp"
namespace minima {
class CoinProof {
    struct Data {
        Coin mCoin;MMRProof mProof;
        Data(Coin coin,MMRProof proof):mCoin(std::move(coin)),mProof(std::move(proof)){}
    };
    std::shared_ptr<Data> mData;
public:
    CoinProof(Coin coin,MMRProof proof):mData(std::make_shared<Data>(std::move(coin),std::move(proof))){}
    const Coin& getCoin() const {return mData->mCoin;}
    Coin& getCoin() {return mData->mCoin;}
    const MMRProof& getMMRProof() const {return mData->mProof;}
    MMRProof& getMMRProof() {return mData->mProof;}
    MMRData getMMRData() const;
    json::JSONObject toJSON() const;
    static std::optional<CoinProof> convertMiniDataVersion(const MiniData& data);
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static CoinProof deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
