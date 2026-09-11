#pragma once
#include "Transaction.hpp"
#include "Witness.hpp"
#include "support/JSONSequence.hpp"
namespace minima {
class TxBody:public Streamable {
public:
    using IDs=json::JSONSequence<std::shared_ptr<MiniData>,true>;
private:
    struct Data {
        std::shared_ptr<MiniData> prng=std::make_shared<MiniData>(MiniData::getRandomData(32));
        std::shared_ptr<MiniData> difficulty=std::make_shared<MiniData>(std::vector<uint8_t>(32,255));
        std::shared_ptr<Transaction> transaction=std::make_shared<Transaction>();
        std::shared_ptr<Witness> witness=std::make_shared<Witness>();
        std::shared_ptr<Transaction> burnTransaction=std::make_shared<Transaction>();
        std::shared_ptr<Witness> burnWitness=std::make_shared<Witness>();
        std::shared_ptr<IDs> ids=std::make_shared<IDs>();
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    // Lvalue accessors model Java public field assignment on the shared object.
    std::shared_ptr<MiniData>& mPRNG() {return mData->prng;}
    std::shared_ptr<MiniData> mPRNG() const {return mData->prng;}
    std::shared_ptr<MiniData>& mTxnDifficulty() {return mData->difficulty;}
    std::shared_ptr<MiniData> mTxnDifficulty() const {return mData->difficulty;}
    std::shared_ptr<Transaction>& mTransaction() {return mData->transaction;}
    std::shared_ptr<Transaction> mTransaction() const {return mData->transaction;}
    std::shared_ptr<Witness>& mWitness() {return mData->witness;}
    std::shared_ptr<Witness> mWitness() const {return mData->witness;}
    std::shared_ptr<Transaction>& mBurnTransaction() {return mData->burnTransaction;}
    std::shared_ptr<Transaction> mBurnTransaction() const {return mData->burnTransaction;}
    std::shared_ptr<Witness>& mBurnWitness() {return mData->burnWitness;}
    std::shared_ptr<Witness> mBurnWitness() const {return mData->burnWitness;}
    std::shared_ptr<IDs>& mTxPowIDList() {return mData->ids;}
    std::shared_ptr<IDs> mTxPowIDList() const {return mData->ids;}
    // Existing TxBody null check, also used when native callers need a reference.
    template<class T> static T& required(const std::shared_ptr<T>& value) {
        if(!value)throw JavaNullPointerException("null TxBody field");return *value;
    }
    void resetRandomPRNG() {mPRNG()=std::make_shared<MiniData>(MiniData::getRandomData(32));}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static TxBody deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
