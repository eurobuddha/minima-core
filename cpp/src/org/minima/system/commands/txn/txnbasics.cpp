#include "txnbasics.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
namespace minima::commands {
txnbasics::txnbasics():Command("txnbasics","[id:] - Automatically set the MMR proofs and scripts for a txn"){}
std::optional<std::string> txnbasics::getFullHelp() const {return std::string(
"\ntxnbasics\n"
"\n"
"Automatically set the MMR proofs and scripts for a transaction.\n"
"\n"
"Only run this when a transaction is ready to be posted.\n"
"\n"
"id:\n"
"    The id of the transaction.\n"
"\n"
"Examples:\n"
"\n"
"txnbasics id:simpletxn\n");}
std::vector<std::string> txnbasics::getValidParams() const {return {"id"};}
json::JSONValue txnbasics::runCommand() {
    auto result=getJSONReply();
    auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();
    auto id=getParam("id");
    auto row=sqlRequired(db).getTransactionRow(id);
    if(!row) throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();auto witness=row->getWitness();
    txnutils::setMMRandScripts(sqlRequired(transaction),sqlRequired(witness),false);
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);
    transaction->calculateTransactionID();
    result.put("response",row->toJSON());return result;
}
std::shared_ptr<Command> txnbasics::getFunction() const {return std::make_shared<txnbasics>();}
}
