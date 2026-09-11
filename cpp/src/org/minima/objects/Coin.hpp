#pragma once
#include "StateVariable.hpp"
#include "Token.hpp"
#include "mmr/MMREntryNumber.hpp"
#include "support/ByteIO.hpp"
#include <functional>
#include <memory>
#include "support/JavaExceptions.hpp"
namespace minima {
class Coin:public Streamable {
    friend class MinimaDB; // Preserve nullable SQL coins through the existing native handle.
public:
    using States=std::vector<StateVariable>;
private:
    // A Coin value is a Java-like object handle. Ordinary copies retain the
    // same coin even when an owning vector grows or removes its element.
    // deepCopy() is the explicit wire-based boundary for independent records.
    struct Data {
        MiniData mCoinID,mAddress;
        MiniNumber mAmount;
        MiniData mTokenID;
        bool mStoreState=true;
        std::shared_ptr<States> mState=std::make_shared<States>();
        MMREntryNumber mMMREntryNumber;
        MiniByte mSpent;
        MiniNumber mBlockCreated;
        std::optional<Token> mToken;
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
    Coin()=default;
    explicit Coin(std::nullptr_t):mData(nullptr){}
    Data& fields() const {if(!mData)throw JavaNullPointerException("null Coin");return *mData;}
    States& states() const {auto& state=fields().mState;if(!state)throw JavaNullPointerException("null coin state");return *state;}
public:
    bool isNull() const {return !mData;}
    static const MiniData COINID_OUTPUT,COINID_ELTOO;
    Coin(MiniData address,MiniNumber amount,MiniData tokenID,bool storeState=true);
    Coin(MiniData coinID,MiniData address,MiniNumber amount,MiniData tokenID,bool storeState=true);
    Coin getSameCoinWithCoinID(MiniData coinID) const;
    void resetCoinID(MiniData coinID) {fields().mCoinID=std::move(coinID);}
    void resetTokenID(MiniData tokenID) {fields().mTokenID=std::move(tokenID);}
    const std::optional<Token>& getToken() const {return fields().mToken;}
    void setToken(std::optional<Token> token) {fields().mToken=std::move(token);}
    void setMMREntryNumber(MMREntryNumber number) {fields().mMMREntryNumber=std::move(number);}
    const MMREntryNumber& getMMREntryNumber() const {return fields().mMMREntryNumber;}
    void setSpent(bool spent) {fields().mSpent=MiniByte(spent);}
    bool getSpent() const {return fields().mSpent.isTrue();}
    void setBlockCreated(MiniNumber block) {fields().mBlockCreated=std::move(block);}
    const MiniNumber& getBlockCreated() const {return fields().mBlockCreated;}
    void setStoreState(bool store) {fields().mStoreState=store;}
    bool storeState() const {return fields().mStoreState;}
    const MiniData& getCoinID() const {return fields().mCoinID;}
    const MiniData& getAddress() const {return fields().mAddress;}
    const MiniNumber& getAmount() const {return fields().mAmount;}
    MiniNumber getTokenAmount() const;
    const MiniData& getTokenID() const {return fields().mTokenID;}
    const States& getState() const {return states();}
    States& getState() {return states();}
    void setState(States state) {fields().mState=std::make_shared<States>(std::move(state));}
    // Owned native bridge for Java's retained ArrayList reference, including null.
    std::shared_ptr<States> getSharedState() const {return fields().mState;}
    void setSharedState(std::shared_ptr<States> state) {fields().mState=std::move(state);}
    bool checkForStateVariable(const std::string& value,bool wildcard=false) const;
    std::string toString() const {return toJSON().toJSONString();}
    json::JSONObject toJSON(bool simpleState=false) const;
    json::JSONObject getStateAsJSON() const;
    static json::JSONObject convertStateListToJSON(const std::vector<StateVariable>& state);
    static std::optional<Coin> convertMiniDataVersion(const MiniData& data);
    static std::optional<Coin> convertMiniDataVersion(const MiniData& data,
        const std::function<void(const wire::IOException&)>& onError);
    Coin deepCopy() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Coin deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
