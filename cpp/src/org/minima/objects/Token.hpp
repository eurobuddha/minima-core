#pragma once
#include "base/MiniData.hpp"
#include "base/MiniNumber.hpp"
#include "base/MiniString.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <optional>
namespace minima {
class Token:public Streamable {
    MiniData mCoinID;
    MiniNumber mTokenScale,mTokenMinimaAmount;
    MiniString mTokenName,mTokenScript;
    MiniNumber mTokenCreated;
    std::optional<MiniData> mTokenID;
    Token()=default;
    void calculateTokenID();
public:
    static const MiniData TOKENID_CREATE,TOKENID_MINIMA;
    Token(MiniData coinID,MiniNumber scale,MiniNumber amount,MiniString name,MiniString script,MiniNumber created=MiniNumber(0));
    MiniNumber getScaledTokenAmount(MiniNumber amount) const;
    MiniNumber getScaledMinimaAmount(MiniNumber amount) const;
    const MiniNumber& getScale() const {return mTokenScale;}
    const MiniNumber& getAmount() const {return mTokenMinimaAmount;}
    MiniNumber getTotalTokens() const {return getScaledTokenAmount(mTokenMinimaAmount);}
    MiniNumber getDecimalPlaces() const;
    const MiniString& getName() const {return mTokenName;}
    const MiniString& getTokenScript() const {return mTokenScript;}
    const MiniData& getCoinID() const {return mCoinID;}
    const MiniNumber& getCreated() const {return mTokenCreated;}
    const std::optional<MiniData>& getTokenID() const {return mTokenID;}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Token deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<Token> convertMiniDataVersion(const MiniData& data);
};
}
