#include "txnclear.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnclear::txnclear():Command("txnclear","[id:] (scripts:) (mmr:) (signatures:) - Clear the Witness data"){}
std::optional<std::string> txnclear::getFullHelp() const {return std::string(
"\ntxnclear\n"
"\n"
"Clear the Witness data - signatures, mmr proofs and script proofs.\n"
"\n"
"id:\n"
"    The id of the transaction to clear.\n"
"\n"
"scripts:\n"
"    Clear the scripts (default : true).\n"
"\n"
"mmr:\n"
"    Clear the MMR proofs (default : true).\n"
"\n"
"signatures:\n"
"    Clear the signatures (default : true).\n"
"\n"
"Examples:\n"
"\n"
"txnclear id:multisig\n");}
std::vector<std::string> txnclear::getValidParams() const {return {"id", "scripts", "mmr", "signatures"};}
json::JSONValue txnclear::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id");bool scripts=getBooleanParam("scripts",true),mmr=getBooleanParam("mmr",true),signatures=getBooleanParam("signatures",true);auto row=db.getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);
    auto transaction=row->getTransaction();if(!transaction)throw std::runtime_error("Null transaction");transaction->clearIsMonotonic();auto witness=row->getWitness();if((scripts||mmr||signatures)&&!witness)throw std::runtime_error("Null witness");if(scripts)witness->clearScriptProofs();if(mmr)witness->clearCoinProofs();if(signatures)witness->clearSignatures();result.put("response",row->toJSON());return result;
}
std::shared_ptr<Command> txnclear::getFunction() const {return std::make_shared<txnclear>();}
}
