#include "TxPoWGenerator.hpp"
#include "TxPoWChecker.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <algorithm>
#include <chrono>
#include <set>
#include <iostream>
#include <bit>
namespace minima {
bool TxPoWGenerator::MEMPOOL_FULL=false;
MiniNumber TxPoWGenerator::MIN_MEMPOOL_BURN(0);
ConsensusServices TxPoWGenerator::services;
MiniData TxPoWGenerator::calculateDifficultyData(const MiniNumber& hashes) {
    auto integer=hashes.getAsBigInteger();if(integer.isZero())throw JavaArithmeticException("BigInteger divide by zero");
    return MiniData(Crypto::MAX_VAL/integer);
}
TxPoW TxPoWGenerator::generateTxPoW(Transaction transaction,Witness witness,std::optional<Transaction> burn,std::optional<Witness> burnWitness) {
    auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return generateTxPoWAt(std::move(transaction),std::move(witness),std::move(burn),std::move(burnWitness),MiniNumber(now));
}
TxPoW TxPoWGenerator::generateTxPoWAt(Transaction transaction,Witness witness,std::optional<Transaction> burn,std::optional<Witness> burnWitness,MiniNumber now) {
    return generateTxPoWAt(std::make_shared<Transaction>(std::move(transaction)),std::make_shared<Witness>(std::move(witness)),
        burn?std::make_shared<Transaction>(std::move(*burn)):nullptr,
        burnWitness?std::make_shared<Witness>(std::move(*burnWitness)):nullptr,std::move(now));
}
TxPoW TxPoWGenerator::generateTxPoW(std::shared_ptr<Transaction> transaction,std::shared_ptr<Witness> witness,
        std::shared_ptr<Transaction> burn,std::shared_ptr<Witness> burnWitness) {
    auto now=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return generateTxPoWAt(std::move(transaction),std::move(witness),std::move(burn),std::move(burnWitness),MiniNumber(now));
}
TxPoW TxPoWGenerator::generateTxPoWAt(std::shared_ptr<Transaction> transaction,std::shared_ptr<Witness> witness,
        std::shared_ptr<Transaction> burn,std::shared_ptr<Witness> burnWitness,MiniNumber now) {
    TxPoW txpow;auto tip=services.getTip();if(!tip) throw JavaNullPointerException("null chain tip");
    txpow.setBlockNumber(tip->getBlockNumber().increment());
    auto minimum=getMedianTimeBlock(tip,std::bit_cast<int32_t>(uint32_t(GlobalParams::MEDIAN_BLOCK_CALC)*2u))->getTxPoW().getTimeMilli();
    auto maximum=minimum.add(TxPoWChecker::MAX_TIME_FUTURE);
    if(now.isLess(minimum)) now=minimum.add(MiniNumber(50000));else if(now.isMore(maximum)) now=maximum;
    txpow.setTimeMilli(now);txpow.setTransaction(std::move(transaction));txpow.setWitness(std::move(witness));
    if(burn) {txpow.setBurnTransaction(std::move(burn));txpow.setBurnWitness(std::move(burnWitness));}
    auto magic=tip->getTxPoW().getMagic().calculateNewCurrent();
    magic.setDesiredKISSVM(services.getMagicDesiredKISSVM());magic.setDesiredMaxTxPoWSize(services.getMagicMaxTxPoWSize());magic.setDesiredMaxTxns(services.getMagicMaxTxns());txpow.setMagic(magic);
    for(int i=0;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) txpow.setSuperParent(i,tip->getTxPoW().getSuperParent(i));
    for(int i=tip->getTxPoW().getSuperLevel();i>=0;--i) txpow.setSuperParent(i,tip->getTxPoW().getTxPoWIDData());
    auto difficulty=getBlockDifficulty(tip);txpow.setBlockDifficulty(difficulty);
    auto minhash=calculateDifficultyData(services.getHashRate());
    if(minhash.isLess(difficulty)) minhash=difficulty;
    if(minhash.isMore(Magic::required(magic.getMinTxPowWork()))) minhash=MiniData(MMREntryNumber::dividePositive(Magic::required(magic.getMinTxPowWork()).getDataValue()*BigInt(10),BigInt(11),16).toBigInteger());
    txpow.setTxDifficulty(minhash);
    auto mempool=services.getAllUnusedTxns();
    std::stable_sort(mempool.begin(),mempool.end(),[](const auto& a,const auto& b) {
        if(!a||!b) throw std::invalid_argument("Null mempool transaction");return a->getBurn().isMore(b->getBurn());
    });
    MEMPOOL_FULL=false;
    for(size_t i=500;i<mempool.size();++i) {
        if(!mempool[i]) throw std::invalid_argument("Null mempool transaction");
        services.removeMemPoolTxPoW(mempool[i]->getTxPoWID());
        if(!MEMPOOL_FULL) {MEMPOOL_FULL=true;MIN_MEMPOOL_BURN=mempool[i]->getBurn();}
    }
    if(mempool.size()>500) mempool.resize(500);
    std::set<std::string> coins;
    auto addCoins=[&](const TxPoW& txn) {for(const Witness* wit:{&txn.getWitness(),&txn.getBurnWitness()}) for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems) coins.insert(proof.getCoin().getCoinID().to0xString());};
    auto collision=[&](const TxPoW& txn) {for(const Witness* wit:{&txn.getWitness(),&txn.getBurnWitness()}) for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems) if(coins.contains(proof.getCoin().getCoinID().to0xString())) return true;return false;};
    addCoins(txpow);std::vector<TxPoW> chosen;
    for(auto& ptr:mempool) {
        if(!ptr) throw std::invalid_argument("Null mempool transaction");auto& mem=*ptr;
        if(!mem.isTransaction()) continue;
        bool valid=true;
        try {
            if(mem.getCheckRejectNumber()>3) valid=false;
            if(valid) {
                if(collision(mem)) {mem.incrementCheckRejectNumber();continue;}
                if(mem.getSizeinBytesWithoutBlockTxns()>Magic::required(magic.getMaxTxPoWSize()).getAsLong()) {
                    MinimaLogger::log("Mempool txn too big.. "+mem.getTxPoWID()+" size:"+std::to_string(mem.getSizeinBytesWithoutBlockTxns())+" max:"+std::to_string(Magic::required(magic.getMaxTxPoWSize()).getAsLong()));valid=false;
                }else if(mem.getTxnDifficulty().isMore(Magic::required(magic.getMinTxPowWork()))) {
                    MinimaLogger::log("Mempool txn TxPoW too low.. "+mem.getTxPoWID());valid=false;
                }
            }
            if(valid&&TxPoWChecker::checkTxPoWSimple(*tip->getMMR(),mem,txpow,false)) {chosen.push_back(mem);txpow.addBlockTxPOW(mem.getTxPoWIDData());addCoins(mem);}
            else mem.incrementCheckRejectNumber();
        } catch(const std::exception& e) {std::clog<<"ERROR Checking TxPoW "<<mem.getTxPoWID()<<' '<<e.what()<<'\n';valid=false;}
        if(!valid) services.removeMemPoolTxPoW(mem.getTxPoWID());
        if(int64_t(chosen.size())>=Magic::required(magic.getMaxNumTxns()).getAsInt()) break;
    }
    txpow.calculateTransactionID();TxPoWTreeNode node(TxBlock(tip->getMMR(),txpow,chosen),false);
    auto root=node.getMMR()->getRoot();if(!root) throw std::invalid_argument("Empty block MMR");txpow.setMMRRoot(root->getData());txpow.setMMRTotal(root->getValue());txpow.calculateTXPOWID();return txpow;
}
MiniData TxPoWGenerator::getBlockDifficulty(std::shared_ptr<TxPoWTreeNode> parent) {
    if(!parent) throw std::invalid_argument("Missing parent");
    if(parent->getBlockNumber().isLess(MiniNumber(8))) return (*Magic::MIN_TXPOW_WORK);
    auto start=getMedianTimeBlock(parent);
    auto end=getMedianTimeBlock(parent->getParent(GlobalParams::MINIMA_BLOCKS_SPEED_CALC.getAsInt()));
    auto blocks=start->getBlockNumber().sub(end->getBlockNumber());
    if(start->getBlockNumber().isEqual(end->getBlockNumber())) return parent->getTxPoW().getBlockDifficulty();
    if(start->getTxPoW().getTimeMilli().sub(end->getTxPoW().getTimeMilli()).isLessEqual(MiniNumber(0))) {
        auto root=services.getRoot();if(!root) throw std::invalid_argument("Missing chain root");
        std::clog<<"SERIOUS NEGATIVE TIME ERROR @ "<<parent->getBlockNumber().toString()<<" Root node : "<<root->getBlockNumber().toString()<<'\n';
        return parent->getTxPoW().getBlockDifficulty();
    }
    auto ratio=GlobalParams::MINIMA_BLOCK_SPEED.div(getChainSpeed(start,blocks));
    if(ratio.isMore(MiniNumber("2.0"))) ratio=MiniNumber("2.0");else if(ratio.isLess(MiniNumber("0.5"))) ratio=MiniNumber("0.5");
    BigInt total(0);int count=0;auto current=start;
    while(count<blocks.getAsInt()) {if(!current) throw std::invalid_argument("Missing difficulty ancestor");total=total+current->getTxPoW().getBlockDifficulty().getDataValue();current=current->getParent();++count;}
    auto average=total/BigInt(count);
    MiniData result(MMREntryNumber(average*ratio.getUnscaled(),ratio.getScale()).toBigInteger());
    return result.isMore((*Magic::MIN_TXPOW_WORK))?(*Magic::MIN_TXPOW_WORK):result;
}
MiniNumber TxPoWGenerator::getChainSpeed(std::shared_ptr<TxPoWTreeNode> start,const MiniNumber& blocksBack) {
    if(!start) throw std::invalid_argument("Missing start block");auto past=start->getParent(blocksBack.getAsInt());
    return start->getBlockNumber().sub(past->getBlockNumber()).div(start->getTxPoW().getTimeMilli().sub(past->getTxPoW().getTimeMilli())).mult(MiniNumber(1000));
}
std::shared_ptr<TxPoWTreeNode> TxPoWGenerator::getMedianTimeBlock(std::shared_ptr<TxPoWTreeNode> start,int32_t blocksBack) {
    std::vector<std::shared_ptr<TxPoWTreeNode>> nodes;
    for(auto current=start;current&&int64_t(nodes.size())<blocksBack;current=current->getParent()) nodes.push_back(current);
    std::stable_sort(nodes.begin(),nodes.end(),[](const auto& a,const auto& b){return a->getTxPoW().getTimeMilli().isLess(b->getTxPoW().getTimeMilli());});
    return nodes.at(nodes.size()/2);
}
void TxPoWGenerator::precomputeTransactionCoinID(Transaction& transaction) {
    const auto inputsOwner=transaction.getAllInputs();const auto& inputs=*inputsOwner;if(inputs.empty()) return;
    const auto base=inputs[0].getCoinID();int number=0;
    for(const auto txnItems=transaction.getAllOutputs();auto& output:*txnItems) {output.resetCoinID(base.isEqual(Coin::COINID_ELTOO)?Coin::COINID_OUTPUT:transaction.calculateCoinID(base,number));++number;}
}
}
