#pragma once
#include "TxnRow.hpp"
#include <functional>
#include "support/JavaExceptions.hpp"
#include "org/minima/database/userprefs/UserDB.hpp"
namespace minima {
class TxnDB {
    std::shared_ptr<UserDB> mUserDB;
    std::function<std::shared_ptr<UserDB>()> mUserResolver;
    std::shared_ptr<UserDB> userDB() const {return mUserResolver?mUserResolver():mUserDB;}
public:
    // Native service boundary, bound to MinimaDB's actual custom database.
    static std::function<std::shared_ptr<TxnDB>()> runtimeLookup;
    static std::shared_ptr<TxnDB> getRuntimeDB(){if(!runtimeLookup)throw std::logic_error("TxnDB requires MinimaDB runtime binding");auto db=runtimeLookup();if(!db)throw JavaNullPointerException("null custom transaction database");return db;}
    using Rows=std::vector<std::shared_ptr<TxnRow>>;
    std::shared_ptr<Rows> mTransactions=std::make_shared<Rows>();
    // Bind the same UserDB instance owned by MinimaDB; no copy of preferences.
    explicit TxnDB(std::shared_ptr<UserDB> userDB=nullptr):mUserDB(std::move(userDB)){}
    void bindUserDB(std::shared_ptr<UserDB> userDB){mUserResolver={};mUserDB=std::move(userDB);}
    void bindUserDBResolver(std::function<std::shared_ptr<UserDB>()> resolver){mUserResolver=std::move(resolver);}
    void loadDB();
    void saveDB();
    void createTransaction(std::optional<std::string> key);
    void addCompleteTransaction(const std::shared_ptr<TxnRow>& row);
    std::shared_ptr<TxnRow> getTransactionRow(const std::optional<std::string>& key) const;
    bool deleteTransaction(const std::optional<std::string>& key);
    std::shared_ptr<Rows> listTxns() const {return mTransactions;}
    void clearTxns(){if(!mTransactions)throw std::invalid_argument("Null transactions");mTransactions->clear();}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxnDB> deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxnDB> convertMiniDataVersion(const MiniData& data);
};
}
