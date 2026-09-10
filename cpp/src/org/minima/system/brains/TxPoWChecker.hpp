#pragma once
#include "support/ConsensusServices.hpp"
namespace minima {
struct TxPoWChecker {
    using TxPoWs=TxBlock::TxPoWs;
    static MiniData CURRENT_NETWORK;
    static MiniNumber MAX_TIME_FUTURE;
    static ConsensusServices services;
    static bool checkTxPoWBlockTimed(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<std::vector<TxPoW>> transactions);
    static bool checkTxPoWBlockTimed(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,std::shared_ptr<TxPoWs> transactions);
    static bool checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,TxPoW& txpow,std::vector<TxPoW>& transactions);
    static bool checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> txpow,const std::shared_ptr<TxPoWs>& transactions);
    static bool checkTxBlockOnly(std::shared_ptr<TxPoWTreeNode> parent,const TxBlock& block);
    static bool checkTxPoWBasic(const TxPoW& txpow);
    static bool checkTxPoWSimple(const MMR& tip,TxPoW& txpow,const TxPoW& block,bool log);
    static bool checkTxPoWSimple(const MMR* tip,TxPoW& txpow,const TxPoW& block,bool log);
    static bool checkTxPoWScripts(const MMR& tip,TxPoW& txpow,const TxPoW& block);
    static bool checkTxPoWScripts(const MMR* tip,TxPoW& txpow,const TxPoW& block);
    static bool checkMMR(const MMR& tip,const TxPoW& txpow,bool log=true);
    static bool checkMMR(const MMR* tip,const TxPoW& txpow,bool log=true);
    static bool checkSignatures(const TxPoW& txpow);
    static bool checkMemPoolCoins(const TxPoW& txpow);
    static bool checkParents(std::shared_ptr<TxPoWTreeNode> tip,const TxPoW& block);
private:
    static bool basic(const std::string& txpowID,const Transaction& transaction,const Witness& witness);
    static bool scripts(Transaction& transaction,Witness& witness,const TxPoW& block);
};
}
