#include "txnmine.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
namespace minima::commands {
txnmine::txnmine():Command("txnmine","(id:) (data:) - Mine a txn but don't post it from either ID or txnexport Data"){}
std::vector<std::string> txnmine::getValidParams() const {return {"id","data"};}
std::shared_ptr<Command> txnmine::getFunction() const {return std::make_shared<txnmine>();}
json::JSONValue txnmine::runCommand() {
    auto result=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();std::shared_ptr<TxnRow> row;
    if(existsParam("id")) {
        auto id=getParam("id");row=sqlRequired(db).getTransactionRow(getParam("id"));
        if(!row)throw CommandException("Transaction not found : "+id);
    }else row=TxnRow::convertMiniDataVersion(getDataParam("data"));
    sqlRequired(sqlRequired(row).getTransaction()).clearIsMonotonic();
    auto transaction=row->getTransaction();auto witness=row->getWitness();
    TxPoWGenerator::precomputeTransactionCoinID(*transaction);transaction->calculateTransactionID();
    auto txpow=std::make_shared<TxPoW>(TxPoWGenerator::generateTxPoW(transaction,witness));txpow->calculateTXPOWID();
    if(!sqlRequired(txnutils::getMiner()).MineMaxTxPoW(false,txpow,120000,false))throw CommandException("FAILED TO MINE txn in 120 seconds !?");
    auto data=MiniData(txpow->serialise());json::JSONObject response;response.put("txpowid",txpow->getTxPoWID());
    response.put("data",data.to0xString());result.put("response",response);return result;
}
}
