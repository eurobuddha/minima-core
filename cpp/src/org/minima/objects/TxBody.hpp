#pragma once
#include "Transaction.hpp"
#include "Witness.hpp"
namespace minima {
class TxBody {
public:
    MiniData mPRNG=MiniData::getRandomData(32);
    MiniData mTxnDifficulty{std::vector<uint8_t>(32,255)};
    std::shared_ptr<Transaction> mTransaction=std::make_shared<Transaction>();
    std::shared_ptr<Witness> mWitness=std::make_shared<Witness>();
    std::shared_ptr<Transaction> mBurnTransaction=std::make_shared<Transaction>();
    std::shared_ptr<Witness> mBurnWitness=std::make_shared<Witness>();
    std::vector<MiniData> mTxPowIDList;
    void resetRandomPRNG() {mPRNG=MiniData::getRandomData(32);}
    json::JSONObject toJSON() const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static TxBody deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
