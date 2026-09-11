#include "txnutils.hpp"
#include "support/AutomaticTransaction.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/commands/CommandException.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
namespace minima::commands {
std::function<std::shared_ptr<TxPoWMiner>()> txnutils::getMiner=[]()->std::shared_ptr<TxPoWMiner>{
    throw JavaNullPointerException("null Main receiver");
};
namespace {
std::shared_ptr<TxPoWTreeNode> proofNode(const std::shared_ptr<TxPoWTreeNode>& tip, const MiniNumber& minimum) {
    auto current=sqlRequired(tip).getBlockNumber();
    auto difference=current.sub(minimum);
    if(difference.isMore(GlobalParams::MINIMA_MMR_PROOF_HISTORY)) difference=GlobalParams::MINIMA_MMR_PROOF_HISTORY;
    return tip->getPastNode(tip->getBlockNumber().sub(difference));
}
ScriptProof scriptProof(const ScriptRow& row) {
    if(!row.getScript()) throw JavaNullPointerException("null script");
    return ScriptProof(*row.getScript());
}
}
void txnutils::setMMRandScripts(Transaction& transaction, Witness& witness, bool exitOnFail) {
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    std::vector<Coin> inputs;
    for(const auto txnItems=transaction.getAllInputs();const auto& coin:*txnItems) {
        if(coin.getCoinID().isEqual(Coin::COINID_ELTOO)) {
            auto current=TxPoWSearcher::getFloatingCoin(tip,coin.getAmount(),coin.getAddress(),coin.getTokenID());
            if(current) inputs.push_back(*current);
            else if(exitOnFail) throw CommandException("Could not find valid unspent coin for "+coin.toJSON().toJSONString());
        } else {
            auto current=TxPoWSearcher::searchCoin(coin.getCoinID());
            if(current) inputs.push_back(*current);
            else if(exitOnFail) throw CommandException("Coin with CoinID not found : "+coin.getCoinID().to0xString());
        }
    }
    auto minimum=MiniNumber::ZERO;
    for(const auto& coin:inputs) if(coin.getBlockCreated().isMore(minimum)) minimum=coin.getBlockCreated();
    auto node=proofNode(tip,minimum);
    if(!node) throw JavaCheckedException("Not enough blocks in chain to make valid MMR Proofs..");
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    for(const auto& coin:inputs) {
        auto proof=sqlRequired(node->getMMR()).getProofToPeak(coin.getMMREntryNumber());
        witness.addCoinProof(CoinProof(coin,std::move(proof)));
        if(!witness.getScript(coin.getAddress())) {
            auto address=coin.getAddress().to0xString();
            auto row=sqlRequired(wallet).getScriptFromAddress(address);
            if(row) witness.addScript(scriptProof(*row));
            else if(exitOnFail) throw JavaCheckedException("SERIOUS ERROR script missing for simple address : "+address);
        }
    }
}
void txnutils::setMMRandScripts(const Coin& coin, Witness& witness) {
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto minimum=MiniNumber::ZERO;
    if(coin.getBlockCreated().isMore(minimum)) minimum=coin.getBlockCreated();
    auto node=proofNode(tip,minimum);
    if(!node) throw JavaCheckedException("Not enough blocks in chain to make valid MMR Proofs..");
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto proof=sqlRequired(node->getMMR()).getProofToPeak(coin.getMMREntryNumber());
    witness.addCoinProof(CoinProof(coin,std::move(proof)));
    auto address=coin.getAddress().to0xString();
    auto row=sqlRequired(wallet).getScriptFromAddress(address);
    if(!row) throw CommandException("SERIOUS ERROR script missing for simple address : "+address);
    witness.addScript(scriptProof(*row));
}
std::shared_ptr<TxnRow> txnutils::createBurnTransaction(const std::vector<std::string>& excluded,
        const MiniData& link, const MiniNumber& amount, bool sign) {
    return automatic_transaction::create(excluded,&link,&amount,sign,false);
}
std::shared_ptr<TxnRow> automatic_transaction::create(const std::vector<std::string>& excluded,
        const MiniData* link,const MiniNumber* suppliedAmount,bool sign,bool validateFirst){
    auto row=std::make_shared<TxnRow>("temp",std::make_shared<Transaction>(),std::make_shared<Witness>());
    auto db=sqlRequired(MinimaDB::getDB()).getTxPoWDB();
    auto miner=txnutils::getMiner();
    auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    if(!suppliedAmount)throw JavaNullPointerException("null automatic transaction amount");
    const auto& amount=*suppliedAmount;
    if(validateFirst&&!amount.isValidMinimaValue())throw CommandException("Invalid Minima amount to send.. "+amount.toString());
    auto relevant=TxPoWSearcher::getRelevantUnspentCoins(tip,"0x00",true);
    auto total=MiniNumber::ZERO;
    std::vector<Coin> coins;
    for(const auto& coin:relevant) {
        auto id=coin.getCoinID().to0xString();
        if(std::find(excluded.begin(),excluded.end(),id)!=excluded.end()) continue;
        if(sqlRequired(miner).checkForMiningCoin(id)) continue;
        if(sqlRequired(db).checkMempoolCoins(coin.getCoinID())) continue;
        coins.push_back(coin);total=total.add(coin.getAmount());
        if(total.isMoreEqual(amount)) break;
    }
    if(total.isLess(amount)) throw CommandException("Not enough funds / coins for the burn..");
    auto change=total.sub(amount);
    auto transaction=row->getTransaction();auto witness=row->getWitness();
    auto minimum=MiniNumber::ZERO;
    for(const auto& coin:coins) {
        transaction->addInput(coin);
        if(coin.getBlockCreated().isMore(minimum)) minimum=coin.getBlockCreated();
    }
    auto node=proofNode(tip,minimum);
    if(!node) throw CommandException("Not enough blocks in chain to make valid MMR Proofs..");
    std::vector<Wallet::Text> keys;
    for(const auto& coin:coins) {
        auto proof=sqlRequired(node->getMMR()).getProofToPeak(coin.getMMREntryNumber());
        witness->addCoinProof(CoinProof(coin,std::move(proof)));
        auto address=coin.getAddress().to0xString();
        auto script=sqlRequired(wallet).getScriptFromAddress(address);
        if(!script) throw CommandException("SERIOUS ERROR script missing for simple address : "+address);
        witness->addScript(scriptProof(*script));
        auto key=script->getPublicKey();
        if(std::find(keys.begin(),keys.end(),key)==keys.end()) keys.push_back(key);
    }
    if(!amount.isValidMinimaValue()) throw CommandException("Invalid Minima amount to send.. "+amount.toString());
    if(change.isMore(MiniNumber::ZERO)) {
        auto address=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getDefaultAddress();
        auto text=sqlRequired(address).getAddress();
        if(!text) throw JavaNullPointerException("null change address");
        transaction->addOutput(Coin(Coin::COINID_OUTPUT,MiniData::fromHex(*text),change,Token::TOKENID_MINIMA,false));
    }
    if(link)transaction->setLinkHash(*link);
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);
    transaction->calculateTransactionID();
    if(sign) for(const auto& key:keys) {
        auto signature=sqlRequired(wallet).signData(key,transaction->getTransactionID().get());
        if(!signature) throw JavaIllegalArgumentException("Cannot add a NULL Signature");
        witness->addSignature(*signature);
    }
    return row;
}
}
