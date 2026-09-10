#pragma once
#include "Greeting.hpp"
#include "support/IBDServices.hpp"
namespace minima {
class IBD {
    std::shared_ptr<Cascade> mCascade;
    using Blocks=std::vector<std::shared_ptr<TxBlock>>;
    std::shared_ptr<Blocks> mTxBlocks=std::make_shared<Blocks>();
    Blocks& blocks() const {if(!mTxBlocks)throw std::invalid_argument("Null IBD blocks");return *mTxBlocks;}
public:
    static const MiniNumber MAX_BLOCKS_FOR_IBD;
    static IBDServices services;
    MiniNumber getTreeRoot() const;
    MiniNumber getTreeTip() const;
    bool createIBD(const Greeting& greeting);
    void createCompleteIBD();
    void createSyncIBD(const TxPoW& lastBlock);
    void createArchiveIBD(const MiniNumber& firstBlock);
    void createArchiveIBD(const MiniNumber& firstBlock,const std::shared_ptr<IBDArchive>& archive,bool useLocal);
    void setCascade(std::shared_ptr<Cascade> cascade){mCascade=std::move(cascade);}
    std::shared_ptr<Cascade> getCascade() const {return mCascade;}
    bool hasCascade() const {return bool(mCascade);}
    bool hasCascadeWithBlocks() const {return mCascade&&mCascade->getLength()>0;}
    std::shared_ptr<Blocks> getTxBlocks() const {return mTxBlocks;}
    void setTxBlocks(std::shared_ptr<Blocks> value){mTxBlocks=std::move(value);}
    bool checkValidData() const;
    BigInt getTotalWeight() const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static IBD deserialise(std::span<const uint8_t> data,size_t& offset);
    static bool checkOurChainHeavier(const IBD& ibd);
    static IBD createShortenedIBD(const IBD& ibd,const std::string& id);
    static void printIBD(const IBD& ibd);
    // Native pure comparison adapter; checkOurChainHeavier first snapshots the DB.
    static bool checkChains(const IBD& current,const IBD& other);
};
}
