#pragma once
#include "org/minima/objects/Transaction.hpp"
#include "org/minima/objects/Witness.hpp"
#include <optional>
namespace minima {
class TxnRow {
    TxnRow()=default;
public:
    std::optional<std::string> mID;
    std::shared_ptr<Transaction> mTransaction;
    std::shared_ptr<Witness> mWitness;
    TxnRow(std::optional<std::string> id,std::shared_ptr<Transaction> transaction,std::shared_ptr<Witness> witness):mID(std::move(id)),mTransaction(std::move(transaction)),mWitness(std::move(witness)){}
    void setID(std::optional<std::string> id){mID=std::move(id);}
    const std::optional<std::string>& getID() const {return mID;}
    std::shared_ptr<Transaction> getTransaction() const {return mTransaction;}
    std::shared_ptr<Witness> getWitness() const {return mWitness;}
    void clearWitness(){mWitness=std::make_shared<Witness>();}
    json::JSONObject toJSON(bool showWitness=true);
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxnRow> deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxnRow> convertMiniDataVersion(const MiniData& data);
};
}
