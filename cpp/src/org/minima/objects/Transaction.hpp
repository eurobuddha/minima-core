#pragma once
#include "Coin.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
class Transaction:public Streamable {
public:
    using Coins=std::vector<Coin>;
    using State=std::vector<StateVariable>;
private:
    struct Data {
        std::shared_ptr<MiniData> linkHash=std::make_shared<MiniData>(MiniData::fromHex("0x00"));
        std::shared_ptr<Coins> inputs=std::make_shared<Coins>(),outputs=std::make_shared<Coins>();
        std::shared_ptr<State> state=std::make_shared<State>();
        std::shared_ptr<MiniData> transactionID=std::make_shared<MiniData>(MiniData::fromHex("0x00"));
        bool haveCheckedMonotonic=false,isMonotonic=false,isValid=false;
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    // Lvalue accessors preserve Java's public mutable flags across copied handles.
    bool& mHaveCheckedMonotonic() {return mData->haveCheckedMonotonic;}
    bool mHaveCheckedMonotonic() const {return mData->haveCheckedMonotonic;}
    bool& mIsMonotonic() {return mData->isMonotonic;}
    bool mIsMonotonic() const {return mData->isMonotonic;}
    bool& mIsValid() {return mData->isValid;}
    bool mIsValid() const {return mData->isValid;}
    void addInput(Coin coin) {mData->inputs->push_back(std::move(coin));}
    void addOutput(Coin coin) {mData->outputs->push_back(std::move(coin));}
    bool isEmpty() const {return mData->inputs->empty()&&mData->outputs->empty();}
    std::shared_ptr<Coins> getAllInputs() const {return mData->inputs;}
    std::shared_ptr<Coins> getAllOutputs() const {return mData->outputs;}
    bool isCheckedMonotonic() const {return mHaveCheckedMonotonic()&&mIsMonotonic();}
    void clearIsMonotonic() {mHaveCheckedMonotonic()=false;}
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
    void clearState() {mData->state->clear();}
    std::shared_ptr<State> getCompleteState() const {return mData->state;}
    std::shared_ptr<MiniData> getLinkHash() const {return mData->linkHash;}
    void setLinkHash(std::shared_ptr<MiniData> link) {mData->linkHash=std::move(link);}
    void setLinkHash(MiniData link) {setLinkHash(std::make_shared<MiniData>(std::move(link)));}
    void calculateTransactionID() const;
    std::shared_ptr<MiniData> getTransactionID() const {return mData->transactionID;}
    // Value adapters check Java null at native call sites requiring a reference.
    const MiniData& transactionIDValue() const {if(!mData->transactionID)throw JavaNullPointerException("null Transaction ID");return *mData->transactionID;}
    const MiniData& linkHashValue() const {if(!mData->linkHash)throw JavaNullPointerException("null Transaction link hash");return *mData->linkHash;}
    MiniData calculateCoinID(const MiniData& baseCoinID,int output) const;
    std::string toString() const {return toJSON().toJSONString();}
    json::JSONObject toJSON() const;
    int64_t calculateStateSize() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Transaction deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
