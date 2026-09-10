#include "txnstate.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnstate::txnstate():Command("txnstate","[id:] [port:] [value:] - Add a state variable"){}
std::optional<std::string> txnstate::getFullHelp() const {return std::string(
"\ntxnstate\n"
"\n"
"Add a state variable to a transaction.\n"
"\n"
"id:\n"
"    The id of the transaction.\n"
"\n"
"port:\n"
"    Port number of the state variable, from 0-255.\n"
"\n"
"value:\n"
"    Value for the state variable.\n"
"\n"
"Examples:\n"
"\n"
"txnstate id:multisig port:0 value:0xFED5..\n"
"\n"
"txnstate id:multisig port:1 value:100 \n"
"\n"
"txnstate id:multisig port:1 value:\"string\" \n");}
std::vector<std::string> txnstate::getValidParams() const {return {"id","port","value"};}
json::JSONValue txnstate::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id"),port=getParam("port"),value=getParam("value");auto row=db.getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);auto transaction=row->getTransaction();
    StateVariable state(javaParseInt(port),value);if(!transaction)throw std::runtime_error("Null transaction");transaction->addStateVariable(std::move(state));transaction->calculateTransactionID();result.put("response",db.getTransactionRow(id)->toJSON());return result;
}
std::shared_ptr<Command> txnstate::getFunction() const {return std::make_shared<txnstate>();}
}
