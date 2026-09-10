#pragma once
#include "support/ChainServices.hpp"
namespace minima {
class TxPoWTreeNode : public std::enable_shared_from_this<TxPoWTreeNode> {
    friend class NIOMessageFixture;
    MiniData mTxPoWTreeID=MiniData::getRandomData(32);
    std::shared_ptr<TxBlock> mTxBlock;
    std::weak_ptr<TxPoWTreeNode> mParent;
    std::vector<std::shared_ptr<TxPoWTreeNode>> mChildren;
    MMREntryNumber mTotalWeight;
    std::shared_ptr<MMR> mMMR;
    std::vector<Coin> mCoins,mComputedRelevantCoins;
    std::vector<MMREntryNumber> mRelevantMMRCoins;
    bool mHaveCheckedFull=false;
    TxPoWTreeNode()=default;
    void constructMMR(bool findRelevant);
    static bool checkRelevant(const Coin& coin);
public:
    static ChainServices services;
    explicit TxPoWTreeNode(std::shared_ptr<TxBlock> block,bool findRelevant=true);
    explicit TxPoWTreeNode(TxBlock block,bool findRelevant=true):TxPoWTreeNode(std::make_shared<TxBlock>(std::move(block)),findRelevant){}
    explicit TxPoWTreeNode(TxPoW testTxPoW);
    void calculateRelevantCoins();
    void addCoinToNode(Coin coin);
    std::shared_ptr<TxBlock> getSharedTxBlock() const {return mTxBlock;}
    TxBlock& getTxBlock() {return *mTxBlock;}
    const TxBlock& getTxBlock() const {return *mTxBlock;}
    std::shared_ptr<TxPoW> getSharedTxPoW() const {return mTxBlock->getSharedTxPoW();}
    TxPoW& getTxPoW() {return mTxBlock->getTxPoW();}
    const TxPoW& getTxPoW() const {return mTxBlock->getTxPoW();}
    const MiniNumber& getBlockNumber() const {return getTxPoW().getBlockNumber();}
    std::shared_ptr<MMR> getMMR() const {return mMMR;}
    std::vector<Coin> getAllCoins() const;
    bool isRelevantEntry(const MMREntryNumber& entry) const;
    std::vector<Coin>& getRelevantCoins() {return mComputedRelevantCoins;}
    const std::vector<Coin>& getRelevantCoins() const {return mComputedRelevantCoins;}
    std::vector<MMREntryNumber>& getRelevantCoinsEntries() {return mRelevantMMRCoins;}
    void removeRelevantCoin(const MMREntryNumber& entry);
    void addChildNode(std::shared_ptr<TxPoWTreeNode> child);
    std::vector<std::shared_ptr<TxPoWTreeNode>>& getChildren() {return mChildren;}
    const std::vector<std::shared_ptr<TxPoWTreeNode>>& getChildren() const {return mChildren;}
    void setParent(std::shared_ptr<TxPoWTreeNode> parent) {mParent=parent;}
    std::shared_ptr<TxPoWTreeNode> getParent() const {return mParent.lock();}
    std::shared_ptr<TxPoWTreeNode> getParent(int32_t blocks);
    std::shared_ptr<TxPoWTreeNode> getPastNode(const MiniNumber& block);
    void copyParentRelevantCoins();
    void clearParent();
    void setTotalWeight(MMREntryNumber weight) {mTotalWeight=std::move(weight);}
    void addToTotalWeight(const MMREntryNumber& weight) {mTotalWeight=mTotalWeight.add(weight);}
    const MMREntryNumber& getTotalWeight() const {return mTotalWeight;}
    bool checkFullTxns(const std::function<bool(const std::string&)>& exists);
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxPoWTreeNode> deserialise(std::span<const uint8_t> data,size_t& offset);
    static void CheckTxBlockForNotifyCoins(const TxBlock& block);
};
}
