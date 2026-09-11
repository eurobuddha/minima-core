#include "TxBlock.hpp"
#include "support/JavaIOExceptionText.hpp"
#include <iostream>
namespace minima {
namespace {
std::shared_ptr<TxBlock::TxPoWs> copyTransactions(const std::vector<TxPoW>& transactions){
    auto result=std::make_shared<TxBlock::TxPoWs>();result->reserve(transactions.size());
    for(const auto& txpow:transactions)result->push_back(std::make_shared<TxPoW>(txpow));return result;
}
}
// Existing value API is retained for native value callers. Runtime callers use
// the shared-object overload below, without copying database-owned TxPoWs.
TxBlock::TxBlock(std::shared_ptr<MMR> parent,TxPoW txpow,const std::vector<TxPoW>& transactions)
    :TxBlock(std::move(parent),std::make_shared<TxPoW>(std::move(txpow)),copyTransactions(transactions)){}
TxBlock::TxBlock(std::shared_ptr<MMR> parent,std::shared_ptr<TxPoW> txpow,const std::shared_ptr<TxPoWs>& transactions):mTxPoW(std::move(txpow)) {
    if(!parent) throw JavaNullPointerException("null parent MMR");
    mPreviousPeaks=parent->getPeaks();MMR copy(parent);calculateCoins(copy,getTxPoW());
    for(const auto& id:getTxPoW().getBlockTransactions()) {
        if(!transactions)throw JavaNullPointerException("null transaction list");
        std::shared_ptr<TxPoW> match;
        for(const auto& candidate:*transactions){
            if(!candidate)throw JavaNullPointerException("null transaction list entry");
            if(candidate->getTxPoWIDData().isEqual(id)){match=candidate;break;}
        }
        if(!match)throw std::invalid_argument("SERIOUS ERROR : TxBlock creation with missing txns.. "+getTxPoW().getTxPoWID()+" missing "+id.to0xString());
        calculateCoins(copy,*match);
    }
}
void TxBlock::calculateCoins(MMR& previous,const TxPoW& txpow) {
    calculateCoins(previous,txpow.getTransaction(),txpow.getWitness());
    calculateCoins(previous,txpow.getBurnTransaction(),txpow.getBurnWitness());
}
void TxBlock::calculateCoins(MMR& previous,const Transaction& txn,const Witness& witness) {
    if(txn.isEmpty()) return;
    const auto spent=witness.getAllCoinProofs();
    for(const auto& input:*spent) {
        const auto& coin=input.getCoin();const auto& entry=coin.getMMREntryNumber();
        previous.updateEntry(entry,input.getMMRProof(),input.getMMRData());
        mSpentCoins.emplace_back(coin,previous.getProofToPeak(entry));
    }
    if(spent->empty()) return;
    const auto& state=txn.getCompleteState();
    const auto& baseID=previous.getBlockTime().isEqual(MiniNumber(1))?txn.getTransactionID():spent->front().getCoin().getCoinID();
    int index=0;
    for(const auto& output:txn.getAllOutputs()) {
        auto id=txn.calculateCoinID(baseID,index++);auto coin=output.getSameCoinWithCoinID(id);
        if(coin.storeState()) coin.setState(state);else mRemovedStates.put(id.to0xString(),state);
        if(output.getTokenID().isEqual(Token::TOKENID_CREATE)) {
            if(!output.getToken())throw JavaNullPointerException("null token creator");
            const auto& creator=*output.getToken();
            Token token(id,creator.getScale(),output.getAmount(),creator.getName(),creator.getTokenScript(),getTxPoW().getBlockNumber());
            coin.resetTokenID(token.getTokenID().value());coin.setToken(std::move(token));
        }
        mNewCoins.push_back(std::move(coin));
    }
}
std::vector<uint8_t> TxBlock::serialise() const {
    auto out=getTxPoW().serialise();wire::append(out,MiniNumber(int64_t(mPreviousPeaks.size())).serialise());
    for(const auto& entry:mPreviousPeaks) wire::append(out,entry->serialise());
    wire::append(out,MiniNumber(int64_t(mSpentCoins.size())).serialise());for(const auto& coin:mSpentCoins) wire::append(out,coin.serialise());
    wire::append(out,MiniNumber(int64_t(mNewCoins.size())).serialise());for(const auto& coin:mNewCoins) wire::append(out,coin.serialise());
    return out;
}
void TxBlock::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mPreviousPeaks.clear();mSpentCoins.clear();mNewCoins.clear();
    mTxPoW=std::make_shared<TxPoW>(TxPoW::deserialise(data,offset));
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i)mPreviousPeaks.push_back(std::make_shared<MMREntry>(MMREntry::deserialise(data,offset)));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mSpentCoins.push_back(CoinProof::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mNewCoins.push_back(Coin::deserialise(data,offset));
}
TxBlock TxBlock::deserialise(std::span<const uint8_t> data,size_t& offset) {
    TxBlock block(std::shared_ptr<TxPoW>{});block.readDataStream(data,offset);return block;
}
std::optional<TxBlock> TxBlock::convertMiniDataVersion(const MiniData& data) {
    return convertMiniDataVersion(data,[](const wire::IOException& error){std::clog<<javaIOExceptionText(error)<<'\n';});
}
std::optional<TxBlock> TxBlock::convertMiniDataVersion(const MiniData& data,const std::function<void(const wire::IOException&)>& log) {
    try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error) {log(error);return std::nullopt;}
}
}
