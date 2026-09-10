#include "txndelete.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txndelete::txndelete():Command("txndelete","[id:] - Delete this custom transaction"){}
std::optional<std::string> txndelete::getFullHelp() const {return std::string(
"\ntxndelete\n"
"\n"
"Delete a previously created custom transaction.\n"
"\n"
"id:\n"
"    The id of the transaction to delete or 'all' to clear ALL transactions.\n"
"\n"
"Examples:\n"
"\n"
"txndelete id:multisig\n"
"\n"
"txndelete id:all\n"
"\n");}
std::vector<std::string> txndelete::getValidParams() const {return {"id"};}
json::JSONValue txndelete::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id");bool found=true;if(id=="all")db.clearTxns();else found=db.deleteTransaction(id);result.put("response",found?"Deleted":"Not found");return result;
}
std::shared_ptr<Command> txndelete::getFunction() const {return std::make_shared<txndelete>();}
}
