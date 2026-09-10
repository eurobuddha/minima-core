#pragma once
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/objects/mmr/MegaMMR.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <mutex>
namespace minima {
class TxPoWSearcher {
    static std::vector<std::shared_ptr<Token>> mImportedTokens;
    static std::recursive_mutex mImportedMutex;
    friend struct TxPoWSearcherFixture;
public:
    struct Services {
        std::function<std::shared_ptr<TxPoWTreeNode>()> getTip;
        std::function<std::shared_ptr<MegaMMR>()> getMegaMMR;
        std::function<void(bool)> readLock;
        std::function<std::shared_ptr<TxPoW>(const std::string&)> getTxPoW;
    };
    static Services services;
    static std::vector<Coin> getAllRelevantUnspentCoins(std::shared_ptr<TxPoWTreeNode> start);
    static std::vector<Coin> getRelevantUnspentCoins(std::shared_ptr<TxPoWTreeNode> start,const std::string& token,bool simpleOnly);
    static std::optional<Coin> searchCoin(const MiniData& id){return searchCoin(id,GeneralParams::IS_MEGAMMR);}
    static std::optional<Coin> searchCoin(const MiniData& id,bool mega);
    static std::optional<Coin> getFloatingCoin(std::shared_ptr<TxPoWTreeNode> start,const MiniNumber& amount,const MiniData& address,const MiniData& token);
    static std::vector<Coin> searchCoins(std::shared_ptr<TxPoWTreeNode> start,bool relevant,bool checkID,const MiniData& id,bool checkAmount,const MiniNumber& amount,bool checkAddress,const MiniData& address,bool checkToken,const MiniData& token,bool simpleOnly,int32_t depth=INT32_MAX);
    static std::vector<Coin> searchCoins(std::shared_ptr<TxPoWTreeNode> start,bool relevant,bool checkID,const MiniData& id,bool checkAmount,const MiniNumber& amount,bool checkAddress,const MiniData& address,bool checkToken,const MiniData& token,bool checkState,const std::string& state,bool wildcard,bool simpleOnly,int32_t depth,bool mega);
    static std::shared_ptr<TxPoWTreeNode> getTreeNodeForCoin(const MiniData& id);
    static std::shared_ptr<TxPoW> getTxPoWBlock(const MiniNumber& block);
    static std::shared_ptr<TxPoW> searchChainForTxPoW(const MiniData& id);
    static std::shared_ptr<TxPoWTreeNode> searchChainForTxPoWBlock(const MiniData& id);
    static std::vector<std::shared_ptr<TxPoW>> searchTxPoWviaAddress(const MiniData& address);
    static bool checkTxPoWForAddress(const TxPoW& txpow,const MiniData& address);
    static bool checkTxPoWRelevant(const TxPoW& txpow,const ChainServices& wallet);
    static std::vector<std::shared_ptr<Token>> getAllTokens();
    static void importToken(std::shared_ptr<Token> token);
    static std::shared_ptr<Token> getToken(const MiniData& id);
};
}
