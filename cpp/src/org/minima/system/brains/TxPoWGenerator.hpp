#pragma once
#include "support/ConsensusServices.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
namespace minima {
class TxPoWGenerator {
    friend class NIOMessageFixture;
    static bool MEMPOOL_FULL;
    static MiniNumber MIN_MEMPOOL_BURN;
public:
    static ConsensusServices services;
    static bool isMempoolFull() {return MEMPOOL_FULL;}
    static MiniNumber getMinMempoolBurn() {return MIN_MEMPOOL_BURN;}
    static MiniData calculateDifficultyData(const MiniNumber& hashes);
    static TxPoW generateTxPoW(Transaction transaction,Witness witness,std::optional<Transaction> burn=std::nullopt,std::optional<Witness> burnWitness=std::nullopt);
    static TxPoW generateTxPoW(std::shared_ptr<Transaction> transaction,std::shared_ptr<Witness> witness,
        std::shared_ptr<Transaction> burn=nullptr,std::shared_ptr<Witness> burnWitness=nullptr);
    static TxPoW generateTxPoWAt(std::shared_ptr<Transaction> transaction,std::shared_ptr<Witness> witness,
        std::shared_ptr<Transaction> burn,std::shared_ptr<Witness> burnWitness,MiniNumber now);
    // Explicit clock adapter permits deterministic replay of the Java generator.
    static TxPoW generateTxPoWAt(Transaction transaction,Witness witness,std::optional<Transaction> burn,std::optional<Witness> burnWitness,MiniNumber now);
    static MiniData getBlockDifficulty(std::shared_ptr<TxPoWTreeNode> parent);
    static MiniNumber getChainSpeed(std::shared_ptr<TxPoWTreeNode> start,const MiniNumber& blocksBack);
    static std::shared_ptr<TxPoWTreeNode> getMedianTimeBlock(std::shared_ptr<TxPoWTreeNode> start,int32_t blocksBack=GlobalParams::MEDIAN_BLOCK_CALC);
    static void precomputeTransactionCoinID(Transaction& transaction);
};
}
