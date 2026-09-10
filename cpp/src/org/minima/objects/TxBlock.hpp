#pragma once
#include "TxPoW.hpp"
#include "mmr/MMR.hpp"
#include "support/JavaHashtable.hpp"
namespace minima {
class TxBlock {
    std::shared_ptr<TxPoW> mTxPoW;
    std::vector<MMR::Entry> mPreviousPeaks;
    std::vector<CoinProof> mSpentCoins;
    std::vector<Coin> mNewCoins;
    JavaHashtable<std::vector<StateVariable>> mRemovedStates;
    void calculateCoins(MMR& previous,const TxPoW& txpow);
    void calculateCoins(MMR& previous,const Transaction& transaction,const Witness& witness);
public:
    using TxPoWs=std::vector<std::shared_ptr<TxPoW>>;
    explicit TxBlock(std::shared_ptr<TxPoW> txpow):mTxPoW(std::move(txpow)){}
    explicit TxBlock(TxPoW txpow):TxBlock(std::make_shared<TxPoW>(std::move(txpow))){}
    TxBlock(std::shared_ptr<MMR> parent,TxPoW txpow,const std::vector<TxPoW>& transactions);
    TxBlock(std::shared_ptr<MMR> parent,std::shared_ptr<TxPoW> txpow,const std::shared_ptr<TxPoWs>& transactions);
    std::shared_ptr<TxPoW> getSharedTxPoW() const {return mTxPoW;}
    TxPoW& getTxPoW() {if(!mTxPoW)throw JavaNullPointerException("null TxBlock TxPoW");return *mTxPoW;}
    const TxPoW& getTxPoW() const {if(!mTxPoW)throw JavaNullPointerException("null TxBlock TxPoW");return *mTxPoW;}
    std::vector<MMR::Entry>& getPreviousPeaks() {return mPreviousPeaks;}
    const std::vector<MMR::Entry>& getPreviousPeaks() const {return mPreviousPeaks;}
    std::vector<CoinProof>& getInputCoinProofs() {return mSpentCoins;}
    const std::vector<CoinProof>& getInputCoinProofs() const {return mSpentCoins;}
    std::vector<Coin>& getOutputCoins() {return mNewCoins;}
    const std::vector<Coin>& getOutputCoins() const {return mNewCoins;}
    const std::vector<StateVariable>* removedState(const std::string& coinID) const {return mRemovedStates.get(coinID);}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static TxBlock deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<TxBlock> convertMiniDataVersion(const MiniData& data);
    static std::optional<TxBlock> convertMiniDataVersion(const MiniData& data,const std::function<void(const wire::IOException&)>& log);
};
}
