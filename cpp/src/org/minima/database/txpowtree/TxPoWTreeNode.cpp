#include "TxPoWTreeNode.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/BlockMMR.hpp"
#include <iostream>
namespace minima {
ChainServices TxPoWTreeNode::services;
TxPoWTreeNode::TxPoWTreeNode(std::shared_ptr<TxBlock> block,bool findRelevant):mTxBlock(std::move(block)) {if(!mTxBlock) throw std::invalid_argument("Null TxBlock");constructMMR(findRelevant);}
TxPoWTreeNode::TxPoWTreeNode(TxPoW testTxPoW):mTxBlock(std::make_shared<TxBlock>(std::move(testTxPoW))),mMMR(std::make_shared<MMR>()){}
bool TxPoWTreeNode::checkRelevant(const Coin& coin) {
    if(services.isAddressRelevant(coin.getAddress().to0xString())) return true;
    for(const auto& state:coin.getState()) if(state.getType().isEqual(StateVariable::STATETYPE_HEX) && (services.isAddressRelevant(state.toString()) || services.isKeyRelevant(state.toString()))) return true;
    return false;
}
namespace {
json::JSONObject coinJSON(const Coin& coin,const TxBlock& block,bool restoreState) {
    auto json=coin.toJSON(true);
    if(restoreState && !coin.storeState()) if(auto state=block.removedState(coin.getCoinID().to0xString())) json.put("state",Coin::convertStateListToJSON(*state));
    return json;
}
json::JSONObject coinEvent(const Coin& coin,const TxBlock& block,bool spent,bool relevant,bool restoreState) {
    json::JSONObject data;data.put(relevant?"relevant":"address",relevant?json::JSONValue(true):json::JSONValue(coin.getAddress().to0xString()));
    data.put("txblockid",block.getTxPoW().getTxPoWID());data.put("txblock",block.getTxPoW().getBlockNumber().toString());data.put("spent",spent);data.put("coin",coinJSON(coin,block,restoreState));return data;
}
}
void TxPoWTreeNode::constructMMR(bool findRelevant) {
    mMMR=std::make_shared<MMR>();bool balancechange=false;size_t outputIndex=0;
    // Java obtains a wallet before applying any state when relevance is enabled.
    if(findRelevant && (!services.isAddressRelevant || !services.isKeyRelevant)) throw std::logic_error("Wallet services are not connected");
    auto process=[&](const Coin& coin,const Coin& relevanceCoin,bool spent) {
        if(!findRelevant) return;
        addCoinToNode(coin);
        if(checkRelevant(relevanceCoin)) {
            mRelevantMMRCoins.push_back(coin.getMMREntryNumber());
            std::clog<<"NEW "<<(spent?"Spent":"Unspent")<<" Coin : "<<coinJSON(coin,*mTxBlock,!spent).toJSONString()<<'\n';
            services.postNotifyEvent("NEWCOIN",coinEvent(coin,*mTxBlock,spent,true,!spent));balancechange=true;
        }
        if(services.checkCoinNotify(coin.getAddress().to0xString())) services.postNotifyEvent("NOTIFYCOIN",coinEvent(coin,*mTxBlock,spent,false,!spent));
    };
    applyBlockMMR(*mMMR,*mTxBlock,[&](const Coin& coin){process(coin,coin,true);},[&](const std::shared_ptr<Coin>& coin){process(*coin,mTxBlock->getOutputCoins().at(outputIndex++),false);},[](const Coin&){return false;});
    mMMR->finalizeSet();if(findRelevant) calculateRelevantCoins();if(balancechange) services.postMessage("MAIN_BALANCE");
}
void TxPoWTreeNode::calculateRelevantCoins() {
    mComputedRelevantCoins.clear();
    for(const auto& coin:getAllCoins()) {
        for(const auto& entry:mRelevantMMRCoins) {
            if(coin.getMMREntryNumber().isEqual(entry)) {
                mComputedRelevantCoins.push_back(coin);
                break;
            }
        }
    }
}
void TxPoWTreeNode::addCoinToNode(Coin coin) {if(GeneralParams::USE_SQL_COINDB) services.insertCoin(mTxPoWTreeID,coin,getBlockNumber());else mCoins.push_back(std::move(coin));}
std::vector<Coin> TxPoWTreeNode::getAllCoins() const {return GeneralParams::USE_SQL_COINDB?services.getAllCoins(mTxPoWTreeID):mCoins;}
bool TxPoWTreeNode::isRelevantEntry(const MMREntryNumber& entry) const {for(const auto& relevant:mRelevantMMRCoins) if(relevant.isEqual(entry)) return true;return false;}
void TxPoWTreeNode::removeRelevantCoin(const MMREntryNumber& entry) {std::erase_if(mRelevantMMRCoins,[&](const auto& relevant){return relevant.isEqual(entry);});}
void TxPoWTreeNode::addChildNode(std::shared_ptr<TxPoWTreeNode> child) {if(!child) throw std::invalid_argument("Null child");child->setParent(shared_from_this());child->getMMR()->setParent(mMMR);mChildren.push_back(std::move(child));}
std::shared_ptr<TxPoWTreeNode> TxPoWTreeNode::getParent(int32_t blocks) {auto parent=shared_from_this();for(int32_t i=0;i<blocks && parent->getParent();++i) parent=parent->getParent();return parent;}
std::shared_ptr<TxPoWTreeNode> TxPoWTreeNode::getPastNode(const MiniNumber& block) {for(auto parent=shared_from_this();parent;parent=parent->getParent()) if(parent->getBlockNumber().isEqual(block)) return parent;return nullptr;}
void TxPoWTreeNode::copyParentRelevantCoins() {
    auto coins=services.getAllRelevantUnspentCoins(getParent());mMMR->setFinalized(false);
    for(const auto& coin:coins) {auto entry=coin.getMMREntryNumber();auto data=mMMR->getEntry(0,entry)->getMMRData();auto proof=mMMR->getProofToPeak(entry);mMMR->updateEntry(entry,proof,data);addCoinToNode(coin);mRelevantMMRCoins.push_back(entry);}
    mMMR->setFinalized(true);calculateRelevantCoins();
}
void TxPoWTreeNode::clearParent() {mParent.reset();mMMR->clearParent();}
bool TxPoWTreeNode::checkFullTxns(const std::function<bool(const std::string&)>& exists) {if(mHaveCheckedFull) return true;for(const auto blockIDs=getTxPoW().getBlockTransactions();const auto txn:TxBody::required(blockIDs)) if(!exists(TxBody::required(txn).to0xString())) return false;mHaveCheckedFull=true;return true;}
std::vector<uint8_t> TxPoWTreeNode::serialise() const {
    auto data=wire::join(*mTxBlock,*mMMR);auto coins=getAllCoins();wire::append(data,MiniNumber(int64_t(coins.size())).serialise());for(const auto& coin:coins) wire::append(data,coin.serialise());
    wire::append(data,MiniNumber(int64_t(mRelevantMMRCoins.size())).serialise());for(const auto& entry:mRelevantMMRCoins) wire::append(data,entry.serialise());return data;
}
void TxPoWTreeNode::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mChildren.clear();mTotalWeight=MMREntryNumber(0);mParent.reset();mRelevantMMRCoins.clear();
    mTxBlock=std::make_shared<TxBlock>(TxBlock::deserialise(data,offset));mMMR=std::make_shared<MMR>(MMR::deserialise(data,offset));
    int32_t count=MiniNumber::deserialise(data,offset).getAsInt();for(int32_t i=0;i<count;++i) addCoinToNode(Coin::deserialise(data,offset));
    count=MiniNumber::deserialise(data,offset).getAsInt();for(int32_t i=0;i<count;++i) mRelevantMMRCoins.push_back(MMREntryNumber::deserialise(data,offset));calculateRelevantCoins();
}
std::shared_ptr<TxPoWTreeNode> TxPoWTreeNode::deserialise(std::span<const uint8_t> data,size_t& offset) {auto node=std::shared_ptr<TxPoWTreeNode>(new TxPoWTreeNode());node->readDataStream(data,offset);return node;}
void TxPoWTreeNode::CheckTxBlockForNotifyCoins(const TxBlock& block) {
    const auto& txpow=block.getTxPoW();auto id=txpow.getTxPoWID();auto number=txpow.getBlockNumber();auto transactions=txpow.getTransactions();
    json::JSONObject blockdata;blockdata.put("txpow",txpow.toJSON());services.postNotifyEvent("NOTIFYCASCADEBLOCK",blockdata);services.addOnChainTxPoW(id,number,id);
    auto notifyTxn=[&](const std::string& txn) {json::JSONObject data;data.put("txblockid",id);data.put("txblock",number.toString());data.put("txpowid",txn);services.postNotifyEvent("NOTIFYCASCADETXN",data);};
    if(txpow.isTransaction()) notifyTxn(id);
    for(const auto& txn:transactions) {notifyTxn(txn);services.addOnChainTxPoW(id,number,txn);}
    for(const auto& input:block.getInputCoinProofs()) {const auto& coin=input.getCoin();if(services.checkCoinNotify(coin.getAddress().to0xString())) services.postNotifyEvent("NOTIFYCASCADECOIN",coinEvent(coin,block,true,false,false));}
    for(const auto& coin:block.getOutputCoins()) if(services.checkCoinNotify(coin.getAddress().to0xString())) services.postNotifyEvent("NOTIFYCASCADECOIN",coinEvent(coin,block,false,false,true));
}
}
