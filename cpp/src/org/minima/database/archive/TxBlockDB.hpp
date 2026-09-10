#pragma once
#include "org/minima/objects/TxBlock.hpp"
#include "support/JavaConcurrentMap.hpp"
namespace minima {
class TxBlockDB {
    std::shared_ptr<JavaConcurrentMap<std::shared_ptr<TxBlock>>> mTxBlockDB=std::make_shared<JavaConcurrentMap<std::shared_ptr<TxBlock>>>();
    mutable std::mutex mLock;
public:
    void addTxBlock(std::shared_ptr<TxBlock> block);
    std::shared_ptr<TxBlock> findTxBlock(const std::string& id) const;
    std::vector<std::shared_ptr<TxBlock>> getChildBlocks(const std::optional<std::string>& id) const;
    void clearAll();
    void clearOld(const MiniNumber& minimum);
};
}
