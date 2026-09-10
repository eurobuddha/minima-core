#include "txncreate.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txncreate::txncreate():Command("txncreate","[id:] - Create a transaction"){}
std::optional<std::string> txncreate::getFullHelp() const {return std::string(
"\ntxncreate\n"
"\n"
"Create a custom transaction.\n"
"\n"
"The first step before defining the inputs and outputs.\n"
"\n"
"id:\n"
"    Create an id for the transaction.\n"
"\n"
"Examples:\n"
"\n"
"txncreate id:multisig\n");}
std::vector<std::string> txncreate::getValidParams() const {return {"id"};}
json::JSONValue txncreate::runCommand(){
    auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id");if(db.getTransactionRow(id))throw CommandException("Txn with this ID already exists : "+id);db.createTransaction(id);auto result=getJSONReply();result.put("response",db.getTransactionRow(id)->toJSON());return result;
}
std::shared_ptr<Command> txncreate::getFunction() const {return std::make_shared<txncreate>();}
}
