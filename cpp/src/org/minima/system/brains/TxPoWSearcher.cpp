#include "TxPoWSearcher.hpp"
#include <bit>
#include "support/JavaExceptions.hpp"
#include <unordered_set>
namespace minima {
TxPoWSearcher::Services TxPoWSearcher::services;
std::vector<std::shared_ptr<Token>> TxPoWSearcher::mImportedTokens;
std::recursive_mutex TxPoWSearcher::mImportedMutex;
namespace {
std::shared_ptr<TxPoW> txpow(const std::shared_ptr<TxPoWTreeNode>& node){return std::shared_ptr<TxPoW>(node->getSharedTxBlock(),&node->getTxPoW());}
std::shared_ptr<Token> token(const Coin& coin){return coin.getToken()?std::make_shared<Token>(*coin.getToken()):nullptr;}
}
std::vector<Coin> TxPoWSearcher::getAllRelevantUnspentCoins(std::shared_ptr<TxPoWTreeNode> start){return searchCoins(start,true,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,false,MiniData::ZERO_TXPOWID,false,MiniData::ZERO_TXPOWID,false);}
std::vector<Coin> TxPoWSearcher::getRelevantUnspentCoins(std::shared_ptr<TxPoWTreeNode> start,const std::string& id,bool simple){return searchCoins(start,true,false,MiniData::ZERO_TXPOWID,false,MiniNumber::ZERO,false,MiniData::ZERO_TXPOWID,true,MiniData::fromHex(id),simple);}
std::optional<Coin> TxPoWSearcher::searchCoin(const MiniData& id,bool mega){auto coins=searchCoins(services.getTip(),false,true,id,false,MiniNumber::ZERO,false,MiniData::ZERO_TXPOWID,false,MiniData::ZERO_TXPOWID,false,"",false,false,INT32_MAX,mega);return coins.empty()||coins.front().isNull()?std::nullopt:std::optional(coins.front());}
std::optional<Coin> TxPoWSearcher::getFloatingCoin(std::shared_ptr<TxPoWTreeNode> start,const MiniNumber& amount,const MiniData& address,const MiniData& token){auto coins=searchCoins(start,false,false,MiniData::ZERO_TXPOWID,true,amount,true,address,true,token,false);return coins.empty()||coins.front().isNull()?std::nullopt:std::optional(coins.front());}
std::vector<Coin> TxPoWSearcher::searchCoins(std::shared_ptr<TxPoWTreeNode> start,bool relevant,bool checkID,const MiniData& id,bool checkAmount,const MiniNumber& amount,bool checkAddress,const MiniData& address,bool checkToken,const MiniData& token,bool simple,int32_t depth){return searchCoins(start,relevant,checkID,id,checkAmount,amount,checkAddress,address,checkToken,token,false,"",false,simple,depth,false);}
std::vector<Coin> TxPoWSearcher::searchCoins(std::shared_ptr<TxPoWTreeNode> start,bool relevant,bool checkID,const MiniData& id,bool checkAmount,const MiniNumber& amount,bool checkAddress,const MiniData& address,bool checkToken,const MiniData& token,bool checkState,const std::string& state,bool wildcard,bool simple,int32_t maxDepth,bool mega){
    std::vector<Coin> result;auto tip=start;std::unordered_set<std::string> spent;int32_t depth=0;bool megaCheck=false;
    while(tip||megaCheck){
        auto previous=depth;depth=std::bit_cast<int32_t>(uint32_t(depth)+1u);if(previous>maxDepth)break;
        std::vector<Coin> coins;
        if(!megaCheck)coins=relevant?tip->getRelevantCoins():tip->getAllCoins();
        else {services.readLock(true);auto owner=services.getMegaMMR();if(!owner)throw JavaNullPointerException("null MegaMMR");for(const auto& coin:owner->getAllCoins().elements()){if(!coin)throw std::runtime_error("Null MegaMMR coin");coins.push_back(*coin);}}
        for(const auto& coin:coins){
            if(checkToken&&!coin.getTokenID().isEqual(token))continue;
            if(checkID&&!coin.getCoinID().isEqual(id))continue;
            if(checkAmount&&!coin.getAmount().isEqual(amount))continue;
            if(checkAddress&&!coin.getAddress().isEqual(address))continue;
            if(checkState&&!coin.checkForStateVariable(state,wildcard))continue;
            auto coinid=coin.getCoinID().to0xString();if(coin.getSpent())spent.insert(coinid);else if(!spent.contains(coinid)){result.push_back(coin.deepCopy());spent.insert(coinid);}
        }
        if(!megaCheck){tip=tip->getParent();if(!tip&&mega)megaCheck=true;}else {services.readLock(false);break;}
    }
    if(simple){std::vector<Coin> filtered;for(const auto& coin:result)if(TxPoWTreeNode::services.isAddressSimple(coin.getAddress().to0xString()))filtered.push_back(coin);return filtered;}return result;
}
std::shared_ptr<TxPoWTreeNode> TxPoWSearcher::getTreeNodeForCoin(const MiniData& id){for(auto tip=services.getTip();tip;tip=tip->getParent())for(const auto& coin:tip->getAllCoins())if(coin.getCoinID().isEqual(id))return tip;return nullptr;}
std::shared_ptr<TxPoW> TxPoWSearcher::getTxPoWBlock(const MiniNumber& block){for(auto tip=services.getTip();tip;tip=tip->getParent())if(tip->getBlockNumber().isEqual(block))return txpow(tip);return nullptr;}
std::shared_ptr<TxPoW> TxPoWSearcher::searchChainForTxPoW(const MiniData& id){for(auto tip=services.getTip();tip;tip=tip->getParent()){auto block=txpow(tip);if(block->getTxPoWIDData().isEqual(id))return block;for(const auto& txn:block->getBlockTransactions())if(txn.isEqual(id))return block;}return nullptr;}
std::shared_ptr<TxPoWTreeNode> TxPoWSearcher::searchChainForTxPoWBlock(const MiniData& id){for(auto tip=services.getTip();tip;tip=tip->getParent())if(tip->getTxPoW().getTxPoWIDData().isEqual(id))return tip;return nullptr;}
std::vector<std::shared_ptr<TxPoW>> TxPoWSearcher::searchTxPoWviaAddress(const MiniData& address){std::vector<std::shared_ptr<TxPoW>> result;
    for(auto tip=services.getTip();tip;tip=tip->getParent())for(const auto& coin:tip->getAllCoins())if(coin.getAddress().isEqual(address)){
        auto block=txpow(tip);if(checkTxPoWForAddress(*block,address))result.push_back(block);
        for(const auto& txn:block->getBlockTransactions()){auto value=services.getTxPoW(txn.to0xString());if(value&&checkTxPoWForAddress(*value,address))result.push_back(value);}
    }
    return result;
}
bool TxPoWSearcher::checkTxPoWForAddress(const TxPoW& txpow,const MiniData& address){
    for(const auto& coin:txpow.getTransaction().getAllInputs())if(coin.getAddress().isEqual(address))return true;
    for(const auto& coin:txpow.getBurnTransaction().getAllInputs())if(coin.getAddress().isEqual(address))return true;
    for(const auto& coin:txpow.getTransaction().getAllOutputs())if(coin.getAddress().isEqual(address))return true;
    for(const auto& coin:txpow.getBurnTransaction().getAllOutputs())if(coin.getAddress().isEqual(address))return true;
    return false;
}
bool TxPoWSearcher::checkTxPoWRelevant(const TxPoW& txpow,const ChainServices& wallet){
    auto relevantState=[&](const std::vector<StateVariable>& state){for(const auto& value:state)if(value.getType().isEqual(StateVariable::STATETYPE_HEX)&&(wallet.isAddressRelevant(value.toString())||wallet.isKeyRelevant(value.toString())))return true;return false;};
    for(const auto& coin:txpow.getTransaction().getAllInputs())if(wallet.isAddressRelevant(coin.getAddress().to0xString())||relevantState(coin.getState()))return true;
    for(const auto& coin:txpow.getBurnTransaction().getAllInputs())if(wallet.isAddressRelevant(coin.getAddress().to0xString())||relevantState(coin.getState()))return true;
    for(const auto& coin:txpow.getTransaction().getAllOutputs())if(wallet.isAddressRelevant(coin.getAddress().to0xString()))return true;
    for(const auto& coin:txpow.getBurnTransaction().getAllOutputs())if(wallet.isAddressRelevant(coin.getAddress().to0xString()))return true;
    return relevantState(txpow.getTransaction().getCompleteState())||relevantState(txpow.getBurnTransaction().getCompleteState());
}
std::vector<std::shared_ptr<Token>> TxPoWSearcher::getAllTokens(){std::lock_guard lock(mImportedMutex);std::vector<std::shared_ptr<Token>> result;std::unordered_set<std::string> added;
    for(auto tip=services.getTip();tip;tip=tip->getParent())for(const auto& coin:tip->getAllCoins()){auto id=coin.getTokenID().to0xString();if(id!="0x00"&&added.insert(id).second)result.push_back(token(coin));}
    for(const auto& item:mImportedTokens){if(!item)throw JavaNullPointerException("Null imported token");auto id=item->getTokenID().value().to0xString();if(added.insert(id).second)result.push_back(item);}return result;
}
void TxPoWSearcher::importToken(std::shared_ptr<Token> token){std::lock_guard lock(mImportedMutex);mImportedTokens.push_back(std::move(token));}
std::shared_ptr<Token> TxPoWSearcher::getToken(const MiniData& id){auto tip=services.getTip();bool megaCheck=false;
    while(tip||megaCheck){std::vector<Coin> coins;if(!megaCheck)coins=tip->getAllCoins();else {services.readLock(true);auto owner=services.getMegaMMR();if(!owner)throw JavaNullPointerException("null MegaMMR");for(const auto& coin:owner->getAllCoins().elements()){if(!coin)throw std::runtime_error("Null MegaMMR coin");coins.push_back(*coin);}}
        // Preserve the pinned early-return lock behavior; node cleanup remains
        // responsible for outstanding locks, as in the Java message runtime.
        for(const auto& coin:coins)if(coin.getTokenID().isEqual(id))return token(coin);
        if(!megaCheck){tip=tip->getParent();if(!tip&&GeneralParams::IS_MEGAMMR)megaCheck=true;}else {services.readLock(false);break;}
    }
    std::lock_guard lock(mImportedMutex);for(const auto& token:mImportedTokens){if(!token)throw std::runtime_error("Null imported token");if(token->getTokenID().value().isEqual(id))return token;}return nullptr;
}
}
