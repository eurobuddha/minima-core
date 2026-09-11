#include "txnpost.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
namespace minima::commands {
txnpost::txnpost():Command("txnpost","[id:] (auto:true) (burn:) (mine:) (txndelete:)- Post a transaction. Automatically set the Scripts and MMR"){}
std::optional<std::string> txnpost::getFullHelp() const {return std::string(
"\ntxnpost\n"
"\n"
"Post a transaction. Automatically set the Scripts and MMR proofs.\n"
"\n"
"Optionally set a burn for the transaction.\n"
"\n"
"id:\n"
"    The id of the transaction.\n"
"\n"
"auto: (optional)\n"
"    Set the scripts and MMR proofs for the transaction.\n"
"\n"
"burn: (optional)\n"
"    Amount in Minima to burn with the transaction.\n"
"\n"
"mine: (optional)\n"
"    true or false - should you mine the transaction immediately.\n"
"\n"
"txndelete: (optional)\n"
"    true or false - delete this txn after posting.\n"
"\n"
"Examples:\n"
"\n"
"txnpost id:simpletxn\n"
"\n"
"txnpost id:simpletxn auto:true burn:0.1\n"
"\n"
"txnpost id:multisig burn:0.1\n");}
std::vector<std::string> txnpost::getValidParams() const {return {"id","auto","burn","mine","txndelete"};}
std::shared_ptr<Command> txnpost::getFunction() const {return std::make_shared<txnpost>();}
json::JSONValue txnpost::runCommand() {
    auto result=getJSONReply();auto id=getParam("id");auto burn=getNumberParam("burn",MiniNumber::ZERO);
    bool automatic=getBooleanParam("auto",false);bool mineSync=getBooleanParam("mine",false);
    auto txpow=postTxn(id,burn,automatic,mineSync);
    bool remove=getBooleanParam("txndelete",false);
    if(remove)sqlRequired(sqlRequired(MinimaDB::getDB()).getCustomTxnDB()).deleteTransaction(id);
    result.put("response",txpow->toJSON());return result;
}
std::shared_ptr<TxPoW> txnpost::postTxn(const std::optional<std::string>& id,const MiniNumber& burn,bool automatic,bool mineSync) {
    auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    if(burn.isLess(MiniNumber::ZERO))throw CommandException("Cannot have negative burn "+burn.toString());
    auto row=sqlRequired(db).getTransactionRow(id);
    if(!row)throw CommandException("Transaction not found : "+id.value_or("null"));
    auto transaction=row->getTransaction();auto witness=row->getWitness();sqlRequired(transaction).clearIsMonotonic();
    if(automatic)txnutils::setMMRandScripts(*transaction,sqlRequired(witness));
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
    std::shared_ptr<TxPoW> txpow;
    if(burn.isMore(MiniNumber::ZERO)) {
        std::vector<std::string> excluded;
        for(const auto witnessItems=sqlRequired(witness).getAllCoinProofs();const auto& proof:*witnessItems)excluded.push_back(proof.getCoin().getCoinID().to0xString());
        auto burned=txnutils::createBurnTransaction(excluded,transaction->transactionIDValue(),burn);
        txpow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness,burned->getTransaction(),burned->getWitness()));
    }else txpow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness));
    txpow->calculateTXPOWID();auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    if(!tip)throw CommandException("Could not find chain tip..");
    auto size=txpow->getSizeinBytesWithoutBlockTxns();auto maximum=tip->getTxPoW().getMagic().getMaxTxPoWSize().getAsLong();
    if(size>maximum)throw CommandException("TxPoW size too large.. "+std::to_string(size)+"/"+std::to_string(maximum));
    auto miner=txnutils::getMiner();
    if(mineSync) {if(!sqlRequired(miner).MineMaxTxPoW(false,txpow,120000))throw CommandException("FAILED TO MINE txn in 120 seconds !?");}
    else sqlRequired(miner).mineTxPoWAsync(txpow);
    return txpow;
}
}
