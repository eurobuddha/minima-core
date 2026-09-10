#pragma once
#include "org/minima/objects/TxBlock.hpp"
#include <functional>
namespace minima {
class TxPoWTreeNode;
// Native dependency boundary for the MinimaDB/Wallet/Main services. Empty
// functions fail on use; they must be connected by the node runtime.
struct ChainServices {
    std::function<bool(const std::string&)> isAddressRelevant,isAddressSimple,isKeyRelevant,checkCoinNotify;
    std::function<void(const std::string&,const json::JSONObject&)> postNotifyEvent;
    std::function<void(const std::string&)> postMessage;
    std::function<void(const MiniData&,const Coin&,const MiniNumber&)> insertCoin;
    std::function<std::vector<Coin>(const MiniData&)> getAllCoins;
    std::function<std::vector<Coin>(std::shared_ptr<TxPoWTreeNode>)> getAllRelevantUnspentCoins;
    std::function<void(const std::string&,const MiniNumber&,const std::string&)> addOnChainTxPoW;
    std::function<void(const TxPoW&)> addSQLTxPoW;
    std::function<void(const TxBlock&)> addTxBlock;
};
}
