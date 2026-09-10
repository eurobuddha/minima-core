#pragma once
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/database/cascade/CascadeNode.hpp"
namespace minima {
// Bound by MinimaDB/UserDB at runtime; missing services fail explicitly.
struct ConsensusServices {
    std::function<std::shared_ptr<TxPoWTreeNode>()> getTip,getRoot;
    std::function<std::shared_ptr<CascadeNode>()> getCascadeTip;
    std::function<bool(const MiniData&)> checkMempoolCoins;
    std::function<std::vector<std::shared_ptr<TxPoW>>()> getAllUnusedTxns;
    std::function<void(const std::string&)> removeMemPoolTxPoW;
    std::function<MiniNumber()> getMagicDesiredKISSVM,getMagicMaxTxPoWSize,getMagicMaxTxns,getHashRate;
};
}
