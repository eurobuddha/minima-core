#include "TxBlockDB.hpp"
namespace minima {
void TxBlockDB::addTxBlock(std::shared_ptr<TxBlock> block){std::lock_guard lock(mLock);if(!block)throw JavaNullPointerException("null TxBlock");auto id=block->getTxPoW().getTxPoWID();mTxBlockDB->put(id,std::move(block));}
std::shared_ptr<TxBlock> TxBlockDB::findTxBlock(const std::string& id) const {std::lock_guard lock(mLock);return mTxBlockDB->get(id).value_or(nullptr);}
std::vector<std::shared_ptr<TxBlock>> TxBlockDB::getChildBlocks(const std::optional<std::string>& id) const {std::lock_guard lock(mLock);std::vector<std::shared_ptr<TxBlock>> result;if(!id)return result;for(const auto& block:mTxBlockDB->elements())if(block->getTxPoW().getParentID().to0xString()==*id)result.push_back(block);return result;}
void TxBlockDB::clearAll(){std::lock_guard lock(mLock);mTxBlockDB->clear();}
void TxBlockDB::clearOld(const MiniNumber& minimum){std::lock_guard lock(mLock);auto next=std::make_shared<JavaConcurrentMap<std::shared_ptr<TxBlock>>>();for(const auto& block:mTxBlockDB->elements())if(block->getTxPoW().getBlockNumber().isMoreEqual(minimum))next->put(block->getTxPoW().getTxPoWID(),block);mTxBlockDB=std::move(next);}
}
