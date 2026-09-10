#include "txnlist.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnlist::txnlist():Command("txnlist","(id:) (transactiononly:) - List current custom transactions"){}
std::optional<std::string> txnlist::getFullHelp() const {return std::string(
"\ntxnlist\n"
"\n"
"List your custom transactions. Includes previously posted transactions.\n"
"\n"
"Returns the full details of transactions.\n"
"\n"
"id: (optional)\n"
"    The id of a single transaction to list.\n"
"\n"
"Examples:\n"
"\n"
"txnlist\n"
"\n"
"txnlist id:multisig\n");}
std::vector<std::string> txnlist::getValidParams() const {return {"id","transactiononly"};}
json::JSONValue txnlist::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id",std::string(""));bool transactionOnly=getBooleanParam("transactiononly",false);
    if(id.empty()){json::JSONArray rows;auto transactions=db.listTxns();if(!transactions)throw std::runtime_error("Null transaction list");for(const auto& row:*transactions){if(!row)throw std::runtime_error("Null transaction row");rows.add(row->toJSON(!transactionOnly));}result.put("response",rows);}
    else {auto row=db.getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);result.put("response",row->toJSON(!transactionOnly));}return result;
}
std::shared_ptr<Command> txnlist::getFunction() const {return std::make_shared<txnlist>();}
}
