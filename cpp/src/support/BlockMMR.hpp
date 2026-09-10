#pragma once
#include "org/minima/objects/TxBlock.hpp"
namespace minima {
// Shared state transition from the inspected MegaMMR.addBlock and
// TxPoWTreeNode.constructMMR. Callers retain their own coin indexes/notifiers.
template<class Spent,class Created,class Prunable>
void applyBlockMMR(MMR& mmr,const TxBlock& block,Spent onSpent,Created onCreated,Prunable prunable) {
    const auto& number=block.getTxPoW().getBlockNumber();mmr.setBlockTime(number);
    for(const auto& peak:block.getPreviousPeaks()) mmr.setEntry(peak->getRow(),peak->getEntryNumber(),peak->getMMRData());
    mmr.calculateEntryNumberFromPeaks();
    for(const auto& input:block.getInputCoinProofs()) {
        auto spent=input.getCoin().deepCopy();spent.setSpent(true);
        mmr.updateEntry(spent.getMMREntryNumber(),input.getMMRProof(),MMRData::CreateMMRDataLeafNode(spent,MiniNumber(0)));
        onSpent(spent);
    }
    for(const auto& output:block.getOutputCoins()) {
        auto coin=std::make_shared<Coin>(output.deepCopy());coin->setMMREntryNumber(mmr.getEntryNumber());coin->setBlockCreated(number);coin->setSpent(false);
        auto data=MMRData::CreateMMRDataLeafNode(*coin,coin->getAmount());data.setUnspendable(prunable(*coin));mmr.addEntry(std::move(data));onCreated(coin);
    }
}
inline MMR constructBlockMMR(const TxBlock& block) {
    MMR mmr;applyBlockMMR(mmr,block,[](const Coin&){},[](const std::shared_ptr<Coin>&){},[](const Coin&){return false;});mmr.finalizeSet();return mmr;
}
}
