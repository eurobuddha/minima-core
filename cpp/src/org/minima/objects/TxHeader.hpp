#pragma once
#include "Magic.hpp"
#include <optional>
namespace minima {
class TxHeader {
public:
    static MiniData MAIN_NET,TEST_NET;
    MiniNumber mNonce;
    MiniData mChainID;
    MiniNumber mTimeMilli;
    MiniNumber mBlockNumber;
    MiniData mBlockDifficulty{std::vector<uint8_t>(32,255)};
    std::vector<MiniData> mSuperParents;
    Magic mMagic;
    MiniData mMMRRoot=MiniData::fromHex("0x00");
    MiniNumber mMMRTotal;
    MiniData mCustomHash=MiniData::fromHex("0x00");
    MiniData mTxBodyHash=MiniData::fromHex("0x00");
    TxHeader();
    const MiniData& getBodyHash() const {return mTxBodyHash;}
    json::JSONObject toJSON() const;
    std::vector<uint8_t> serialise() const;
    static TxHeader deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<TxHeader> convertMiniDataVersion(const MiniData& data);
};
}
