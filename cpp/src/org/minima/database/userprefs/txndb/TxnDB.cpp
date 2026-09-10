#include "TxnDB.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
namespace minima {
std::function<std::shared_ptr<TxnDB>()> TxnDB::runtimeLookup;
namespace {
bool sameID(const std::shared_ptr<TxnRow>& row,const std::optional<std::string>& key){if(!row)throw std::invalid_argument("Null transaction row");auto id=unicode::decodeUTF8(row->getID().value());return key&&id==unicode::decodeUTF8(*key);}
}
void TxnDB::loadDB(){auto user=userDB();if(!user)throw std::logic_error("TxnDB requires UserDB binding");auto data=user->loadCustomTransactions();if(!data.isEqual(MiniData::ZERO_TXPOWID)){auto db=convertMiniDataVersion(data);if(!db)throw std::invalid_argument("Invalid saved transaction DB");mTransactions=db->mTransactions;}}
void TxnDB::saveDB(){auto user=userDB();if(!user)throw std::logic_error("TxnDB requires UserDB binding");user->saveCustomTransactions(MiniData(serialise()));}
void TxnDB::createTransaction(std::optional<std::string> key){if(!mTransactions)throw std::invalid_argument("Null transactions");mTransactions->push_back(std::make_shared<TxnRow>(std::move(key),std::make_shared<Transaction>(),std::make_shared<Witness>()));}
void TxnDB::addCompleteTransaction(const std::shared_ptr<TxnRow>& row){if(!row)throw std::invalid_argument("Null transaction row");deleteTransaction(row->getID());mTransactions->push_back(row);}
std::shared_ptr<TxnRow> TxnDB::getTransactionRow(const std::optional<std::string>& key) const {if(!mTransactions)throw std::invalid_argument("Null transactions");for(const auto& row:*mTransactions)if(sameID(row,key))return row;return nullptr;}
bool TxnDB::deleteTransaction(const std::optional<std::string>& key){if(!mTransactions)throw std::invalid_argument("Null transactions");bool found=false;auto rows=std::make_shared<Rows>();for(const auto& row:*mTransactions){if(!sameID(row,key))rows->push_back(row);else found=true;}mTransactions=std::move(rows);return found;}
std::vector<uint8_t> TxnDB::serialise() const {if(!mTransactions)throw std::invalid_argument("Null transactions");auto result=MiniNumber(int64_t(mTransactions->size())).serialise();for(const auto& row:*mTransactions){if(!row)throw std::invalid_argument("Null transaction row");wire::append(result,row->serialise());}return result;}
void TxnDB::readDataStream(std::span<const uint8_t> data,size_t& offset){mTransactions=std::make_shared<Rows>();auto count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mTransactions->push_back(TxnRow::deserialise(data,offset));}
std::shared_ptr<TxnDB> TxnDB::deserialise(std::span<const uint8_t> data,size_t& offset){auto db=std::make_shared<TxnDB>();db->readDataStream(data,offset);return db;}
std::shared_ptr<TxnDB> TxnDB::convertMiniDataVersion(const MiniData& data){try{size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& e){std::clog<<"TxnDB read error: "<<e.what()<<'\n';return nullptr;}}
}
