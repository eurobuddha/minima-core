#include "TxPoWChecker.hpp"
#include "TxPoWGenerator.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/system/network/minima/RelayPolicy.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <set>
#include "support/NativeThrowable.hpp"
#include "support/JavaConcurrentMap.hpp"
#include <iostream>
#include <bit>
namespace minima {
MiniData TxPoWChecker::CURRENT_NETWORK=MiniData::fromHex("0x00");
MiniNumber TxPoWChecker::MAX_TIME_FUTURE(86400000);
ConsensusServices TxPoWChecker::services;
namespace {
bool headerValid(std::shared_ptr<TxPoWTreeNode> parent,const TxPoW& txpow) {
    auto reject=[&](const std::string& text){MinimaLogger::log(text);return false;};
    if(!txpow.getChainID().isEqual(TxPoWChecker::CURRENT_NETWORK))return reject("Invalid Block ChainID! "+txpow.getChainID().to0xString()+" "+txpow.getTxPoWID());
    if(!parent) throw JavaNullPointerException("null parent node");
    if(!txpow.getBlockNumber().isEqual(parent->getBlockNumber().increment()))return reject("Invalid TxPoW block with wrong blocknumber "+txpow.getTxPoWID());
    if(!TxPoWChecker::checkParents(parent,txpow))return reject("Invalid TxPoW Super Parents "+txpow.getTxPoWID());
    auto median=TxPoWGenerator::getMedianTimeBlock(parent,std::bit_cast<int32_t>(uint32_t(GlobalParams::MEDIAN_BLOCK_CALC)*2u));
    auto minimum=median->getTxPoW().getTimeMilli();
    if(txpow.getTimeMilli().isLess(minimum))return reject("Invalid TxPoW TimeMilli less than median 1 hr back "+txpow.getTxPoWID());
    if(txpow.getTimeMilli().isMore(minimum.add(TxPoWChecker::MAX_TIME_FUTURE)))return reject("Invalid TxPoW TimeMilli more than 24 hrs in future "+txpow.getTxPoWID());
    if(!txpow.getBlockDifficulty().isEqual(TxPoWGenerator::getBlockDifficulty(parent)))return reject("Incorrect TxPoW block difficulty @ "+txpow.getBlockNumber().toString()+" "+txpow.getTxPoWID());
    if(!txpow.getMagic().checkSame(parent->getTxPoW().getMagic().calculateNewCurrent()))return reject("Incorrect Magic values "+std::to_string(txpow.getBlockTransactions().size())+" "+txpow.getTxPoWID());
    return true;
}
}
bool TxPoWChecker::checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,TxPoW& txpow,std::vector<TxPoW>& transactions) {
    // Synchronous references cannot outlive this invocation; preserve cache
    // mutations on the caller's original values, as the script runner does.
    auto borrowed=std::make_shared<TxPoWs>();borrowed->reserve(transactions.size());
    for(auto& txn:transactions)borrowed->emplace_back(&txn,[](TxPoW*){});
    return checkTxPoWBlock(std::move(parent),std::shared_ptr<TxPoW>(&txpow,[](TxPoW*){}),borrowed);
}
bool TxPoWChecker::checkTxPoWBlock(std::shared_ptr<TxPoWTreeNode> parent,std::shared_ptr<TxPoW> ownedTxPoW,const std::shared_ptr<TxPoWs>& transactions) {
    try {
        if(!ownedTxPoW)throw JavaNullPointerException("null block TxPoW");auto& txpow=*ownedTxPoW;
        if(!headerValid(parent,txpow)) return false;
        if(int64_t(txpow.getBlockTransactions().size())>txpow.getMagic().getMaxNumTxns().getAsInt()){
            MinimaLogger::log("Too many transactions in block "+std::to_string(txpow.getBlockTransactions().size())+" "+txpow.getTxPoWID());return false;
        }
        // Java gathers every main/burn proof before checking uniqueness. A later
        // invalid transaction must still be visited when an earlier ID repeats.
        std::vector<std::string> allCoins;
        auto appendCoins=[&](const TxPoW& value){
            if(value.isTransaction())for(const Witness* wit:{&value.getWitness(),&value.getBurnWitness()})
                for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems)allCoins.push_back(proof.getCoin().getCoinID().to0xString());
        };
        appendCoins(txpow);
        if(!transactions)throw JavaNullPointerException("null block transaction list");
        for(const auto& txn:*transactions){if(!txn)throw JavaNullPointerException("null block transaction");appendCoins(*txn);}
        std::set<std::string> coins(allCoins.begin(),allCoins.end());
        if(coins.size()!=allCoins.size()){
            MinimaLogger::log("Invalid TxPoW Block with non unique CoinIDs "+txpow.getTxPoWID());return false;
        }
        auto mmr=parent->getMMR();
        if(txpow.isTransaction()&&!checkTxPoWSimple(mmr.get(),txpow,txpow,true)) return false;
        for(auto& txn:*transactions) if(!checkTxPoWSimple(mmr.get(),*txn,txpow,true)) return false;
        TxPoWTreeNode node(TxBlock(mmr,ownedTxPoW,transactions),false);
        auto root=node.getMMR()->getRoot();
        if(!root) throw JavaNullPointerException("null block MMR root");
        if(!root->getData().isEqual(txpow.getMMRRoot())||!root->getValue().isEqual(txpow.getMMRTotal())){
            MinimaLogger::log("ERROR : MMR in TxPOW block and calculated don't match! @ "+txpow.getBlockNumber().toString()+" "+txpow.getTxPoWID());return false;
        }
        return true;
    } catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& e) {MinimaLogger::log("ERROR checking TxPoW Block..");MinimaLogger::log(std::make_shared<NativeThrowable>(e));return false;}
}
bool TxPoWChecker::checkTxBlockOnly(std::shared_ptr<TxPoWTreeNode> parent,const TxBlock& block) {
    try {return headerValid(parent,block.getTxPoW());}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& e) {MinimaLogger::log("ERROR checking TxBlock in slave node..");MinimaLogger::log(std::make_shared<NativeThrowable>(e));return false;}
}
bool TxPoWChecker::checkTxPoWBasic(const TxPoW& txpow) {
    if(!txpow.getChainID().isEqual(CURRENT_NETWORK)){
        MinimaLogger::log("Wrong TxPoW ChainID! "+txpow.getChainID().to0xString()+" "+txpow.getTxPoWID());return false;
    }
    std::vector<std::string> allCoins;
    if(txpow.isTransaction())for(const Witness* wit:{&txpow.getWitness(),&txpow.getBurnWitness()})
        for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems)allCoins.push_back(proof.getCoin().getCoinID().to0xString());
    JavaHashMap<bool> coins;coins.reserveForPutAll(std::max(size_t(12),allCoins.size()));
    for(const auto& coin:allCoins)coins.put(coin,true);
    if(coins.size()!=allCoins.size()){
        MinimaLogger::log("Invalid TxPoW Transaction / Burn with non unique CoinIDs "+txpow.getTxPoWID()+" uniquesize:"+std::to_string(coins.size())+" txncoins:"+std::to_string(allCoins.size()));
        for(const auto& coin:coins.keys())MinimaLogger::log("[!] Unique coinid : "+coin);
        for(const auto& coin:allCoins)MinimaLogger::log("[!] All coinid : "+coin);
        return false;
    }
    const auto& transaction=txpow.getTransaction();const auto& burn=txpow.getBurnTransaction();
    if(!basic(txpow.getTxPoWID(),transaction,txpow.getWitness())) return false;
    if(!transaction.linkHashValue().isEqual(MiniData::ZERO_TXPOWID)){
        MinimaLogger::log("Invalid LinkHash for Transaction ( NOT 0x00 ) "+txpow.getTxPoWID());return false;
    }
    if(!burn.isEmpty()) {
        if(!burn.linkHashValue().isEqual(transaction.transactionIDValue())){
            MinimaLogger::log("Invalid LinkHash for Burn Transaction "+txpow.getTxPoWID());return false;
        }
        return basic(txpow.getTxPoWID(),burn,txpow.getBurnWitness());
    }
    return true;
}
bool TxPoWChecker::basic(const std::string& txpowID,const Transaction& transaction,const Witness& witness) {
    auto reject=[](const std::string& text){MinimaLogger::log(text);return false;};
    if(transaction.isEmpty()) return true;
    if(!transaction.checkValid())return reject("Invalid Transaction Inputs and Outputs.. "+transaction.toJSON().toJSONString());
    const auto inputsOwner=transaction.getAllInputs();const auto& inputs=*inputsOwner;
    if(inputs.empty())return reject("Transaction MUST have at least 1 input @ "+txpowID);
    const auto proofs=witness.getAllCoinProofs();
    if(inputs.size()!=proofs->size())return reject("Wrong Number of MMR Proofs Inputs:"+std::to_string(inputs.size())+" MMRProofs:"+std::to_string(proofs->size())+" @ "+txpowID);
    std::set<std::string> used;
    for(size_t i=0;i<inputs.size();++i) {
        const auto& input=inputs[i];const auto& coin=(*proofs)[i].getCoin();
        if(!used.insert(coin.getCoinID().to0xString()).second)return reject("CoinID used more than once @ "+std::to_string(i)+" in "+txpowID);
        bool amount=input.getAmount().isEqual(coin.getAmount()),address=input.getAddress().isEqual(coin.getAddress()),token=input.getTokenID().isEqual(coin.getTokenID());
        if(!amount||!address||!token)return reject("Input coin details don't match coinproof "+txpowID);
        if(!input.getCoinID().isEqual(Coin::COINID_ELTOO)&&!input.getCoinID().isEqual(coin.getCoinID()))return reject("CoinID input "+std::to_string(i)+" doesn't match proof "+txpowID);
        if(!input.getTokenID().isEqual(Token::TOKENID_MINIMA)) {
            if(!input.getToken()||!input.getToken()->getTokenID())throw JavaNullPointerException("null input token/tokenid");
            if(!input.getTokenID().isEqual(*input.getToken()->getTokenID()))return reject("TokenID in Coin input "+std::to_string(i)+" doesn't match token "+txpowID);
            if(!coin.getToken()||!coin.getToken()->getTokenID())throw JavaNullPointerException("null proof token/tokenid");
            if(!coin.getTokenID().isEqual(*coin.getToken()->getTokenID()))return reject("TokenID in MMR Proof input "+std::to_string(i)+" doesn't match token "+txpowID);
        }
        if(coin.getSpent())return reject("Trying to spend spent coin..");
        if(!witness.getScript(input.getAddress()))return reject("Script Missing from TxPoW for address "+input.getAddress().to0xString());
    }
    for(const auto txnItems=transaction.getAllOutputs();const auto& coin:*txnItems) {
        if(!coin.getTokenID().isEqual(Token::TOKENID_MINIMA)&&!coin.getTokenID().isEqual(Token::TOKENID_CREATE)) {
            if(!coin.getToken())return reject("Incorrect output token with NULL token..");
            if(!coin.getToken()->getTokenID())throw JavaNullPointerException("null output tokenid");
            if(!coin.getToken()->getTokenID()->isEqual(coin.getTokenID()))return reject("Incorrect output token with different tokenid..");
        }else if(coin.getTokenID().isEqual(Token::TOKENID_CREATE)&&!coin.getToken())return reject("Incorrect output token for create token with NULL token..");
    }
    return true;
}
bool TxPoWChecker::checkTxPoWSimple(const MMR& tip,TxPoW& txpow,const TxPoW& block,bool log) {
    return checkTxPoWSimple(&tip,txpow,block,log);
}
bool TxPoWChecker::checkTxPoWSimple(const MMR* tip,TxPoW& txpow,const TxPoW& block,bool log) {
    if(txpow.getTxnDifficulty().isMore(block.getMagic().getMinTxPowWork())){MinimaLogger::log("TxPoW difficulty too low.. "+txpow.getTxPoWID());return false;}
    auto maximum=block.getMagic().getMaxTxPoWSize().getAsLong();
    auto size=txpow.getSizeinBytesWithoutBlockTxns();
    if(size>maximum){MinimaLogger::log("TxPoW size too large.. "+std::to_string(size)+" "+txpow.getTxPoWID());return false;}
    if(!RelayPolicy::checkMaxStateStoreSize(txpow,maximum)){MinimaLogger::log("TxPoW state store too large..");return false;}
    return checkMMR(tip,txpow,log)&&checkTxPoWScripts(tip,txpow,block);
}
bool TxPoWChecker::checkTxPoWScripts(const MMR& tip,TxPoW& txpow,const TxPoW& block) {
    return checkTxPoWScripts(&tip,txpow,block);
}
bool TxPoWChecker::checkTxPoWScripts(const MMR*,TxPoW& txpow,const TxPoW& block) {
    return scripts(txpow.getTransaction(),txpow.getWitness(),block)&&scripts(txpow.getBurnTransaction(),txpow.getBurnWitness(),block);
}
bool TxPoWChecker::scripts(Transaction& transaction,Witness& witness,const TxPoW& block) {
    if(transaction.isCheckedMonotonic()) return transaction.mIsValid();
    transaction.mHaveCheckedMonotonic()=true;transaction.mIsMonotonic()=true;transaction.mIsValid()=false;
    if(transaction.isEmpty()) {transaction.mIsValid()=true;return true;}
    auto maxops=block.getMagic().getMaxKISSOps().getAsInt();
    const auto proofs=witness.getAllCoinProofs();
    if(proofs->size()!=transaction.getAllInputs()->size()){MinimaLogger::log("Wrong number of MMRProofs("+std::to_string(proofs->size())+") for Inputs("+std::to_string(transaction.getAllInputs()->size())+")");return false;}
    // Contracts execute synchronously; aliases cannot escape this invocation.
    auto txn=std::shared_ptr<Transaction>(&transaction,[](Transaction*){});
    auto wit=std::shared_ptr<Witness>(&witness,[](Witness*){});
    for(size_t i=0;i<proofs->size();++i) {
        const auto& coin=(*proofs)[i].getCoin();auto proof=witness.getScript(coin.getAddress());
        if(!proof){MinimaLogger::log("Script NOT found for address : "+coin.getAddress().to0xString());return false;}
        auto run=[&](const std::string& script,bool tokenScript) {
            kissvm::Contract contract(script,witness.getAllSignatureKeys(),wit,txn,coin.getState());
            contract.setMaxInstructions(maxops);contract.setGlobals(block.getBlockNumber(),block.getTimeMilli(),transaction,int(i),coin.getBlockCreated(),script);contract.run();
            if(!contract.isMonotonic()) transaction.mIsMonotonic()=false;
            if(!contract.isSuccess())MinimaLogger::log(std::string(tokenScript?"Token Script FAIL input:":"Script FAIL input:")+std::to_string(i)+" "+(GeneralParams::SCRIPTLOGS?contract.getCompleteTraceLog():script));
            return contract.isSuccess();
        };
        if(!run(proof->getScript().toString(),false)) return false;
        if(!coin.getTokenID().isEqual(Token::TOKENID_MINIMA)) {
            if(!coin.getToken())throw JavaNullPointerException("null script token");
            auto script=coin.getToken()->getTokenScript().toString();
            size_t begin=0,end=script.size();while(begin<end&&uint8_t(script[begin])<=32) ++begin;while(end>begin&&uint8_t(script[end-1])<=32) --end;
            script=script.substr(begin,end-begin);
            if(script!="RETURN TRUE"&&!run(script,true)) return false;
        }
    }
    transaction.mIsValid()=true;return true;
}
bool TxPoWChecker::checkMMR(const MMR& tip,const TxPoW& txpow,bool log) {
    return checkMMR(&tip,txpow,log);
}
bool TxPoWChecker::checkMMR(const MMR* tip,const TxPoW& txpow,bool log) {
    for(const Witness* wit:{&txpow.getWitness(),&txpow.getBurnWitness()}) for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems) {
        const auto& coin=proof.getCoin();auto leaf=MMRData::CreateMMRDataLeafNode(coin,coin.getAmount());
        if(!tip) throw JavaNullPointerException("null MMR receiver");
        if(!tip->checkProofTimeValid(coin.getMMREntryNumber(),leaf,proof.getMMRProof())) {
            if(log)MinimaLogger::log("Invalid MMR Proof! @ "+tip->getBlockTime().toString());return false;
        }
    }
    return true;
}
bool TxPoWChecker::checkSignatures(const TxPoW& txpow) {
    auto check=[&](const Transaction& txn,const Witness& wit) {
        const auto transactionID=txn.getTransactionID();
        for(const auto witnessItems=wit.getAllSignatures();const auto& sig:*witnessItems) {TreeKey key;key.setPublicKey(sig.getRootPublicKey());if(!key.verify(transactionID.get(),sig)){MinimaLogger::log("SIGNATURE FAIL : "+txpow.getTxPoWID());return false;}}
        return true;
    };
    return check(txpow.getTransaction(),txpow.getWitness())&&check(txpow.getBurnTransaction(),txpow.getBurnWitness());
}
bool TxPoWChecker::checkMemPoolCoins(const TxPoW& txpow) {
    for(const Witness* wit:{&txpow.getWitness(),&txpow.getBurnWitness()}) for(const auto witnessItems=wit->getAllCoinProofs();const auto& proof:*witnessItems) if(services.checkMempoolCoins(proof.getCoin().getCoinID())) return true;
    return false;
}
bool TxPoWChecker::checkParents(std::shared_ptr<TxPoWTreeNode> tip,const TxPoW& block) {
    int level=0;auto check=[&](const TxPoW& txpow) {
        while(txpow.getSuperLevel()>=level) {if(!block.getSuperParent(level).isEqual(txpow.getTxPoWIDData())) return false;++level;}return true;
    };
    for(auto current=tip;current;current=current->getParent()) if(!check(current->getTxPoW())) return false;
    for(auto current=services.getCascadeTip();current;current=current->getParent()) if(!check(current->getTxPoW())) return false;
    for(int i=level;i<GlobalParams::MINIMA_CASCADE_LEVELS;++i) if(!block.getSuperParent(i).isEqual(MiniData::ZERO_TXPOWID)) return false;
    return true;
}
}
