#pragma once
#include "Coin.hpp"
namespace minima {
class Transaction {
    MiniData mLinkHash=MiniData::fromHex("0x00");
    std::vector<Coin> mInputs,mOutputs;
    std::vector<StateVariable> mState;
    mutable MiniData mTransactionID=MiniData::fromHex("0x00");
public:
    bool mHaveCheckedMonotonic=false,mIsMonotonic=false,mIsValid=false;
    void addInput(Coin coin) {mInputs.push_back(std::move(coin));}
    void addOutput(Coin coin) {mOutputs.push_back(std::move(coin));}
    bool isEmpty() const {return mInputs.empty()&&mOutputs.empty();}
    const std::vector<Coin>& getAllInputs() const {return mInputs;}
    std::vector<Coin>& getAllInputs() {return mInputs;}
    const std::vector<Coin>& getAllOutputs() const {return mOutputs;}
    std::vector<Coin>& getAllOutputs() {return mOutputs;}
    bool isCheckedMonotonic() const {return mHaveCheckedMonotonic&&mIsMonotonic;}
    void clearIsMonotonic() {mHaveCheckedMonotonic=false;}
    MiniNumber sumInputs() const;
    MiniNumber sumInputs(const MiniData& tokenID) const;
    MiniNumber sumOutputs() const;
    MiniNumber sumOutputs(const MiniData& tokenID) const;
    bool checkValid() const;
    MiniNumber getBurn() const;
    void addStateVariable(StateVariable state);
    void removeStateVariable(int port);
    const StateVariable* getStateValue(int port) const;
    bool stateExists(int port) const {return getStateValue(port)!=nullptr;}
    void clearState() {mState.clear();}
    const std::vector<StateVariable>& getCompleteState() const {return mState;}
    std::vector<StateVariable>& getCompleteState() {return mState;}
    const MiniData& getLinkHash() const {return mLinkHash;}
    void setLinkHash(MiniData link) {mLinkHash=std::move(link);}
    void calculateTransactionID() const;
    const MiniData& getTransactionID() const {return mTransactionID;}
    MiniData calculateCoinID(const MiniData& baseCoinID,int output) const;
    std::string toString() const {return toJSON().toJSONString();}
    json::JSONObject toJSON() const;
    int64_t calculateStateSize() const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static Transaction deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
