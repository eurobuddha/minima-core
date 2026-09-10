#include "txnexport.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnexport::txnexport():Command("txnexport","[id:] (file:) - Export a transaction as HEX or to a file"){}
std::optional<std::string> txnexport::getFullHelp() const {return std::string(
"\ntxnexport\n"
"\n"
"Export a transaction as HEX or to a file.\n"
"\n"
"The output can then be imported using 'txnimport' to another node. E.g. For signing.\n"
"\n"
"id:\n"
"    The id of the transaction to export.\n"
"\n"
"file: (optional)\n"
"    File name/path to export the transaction to, must use the .txn extension.\n"
"\n"
"Examples:\n"
"\n"
"txnexport id:simpletxn\n"
"\n"
"txnexport id:multisig file:multisig.txn\n");}
std::vector<std::string> txnexport::getValidParams() const {return {"id","file","showtxn"};}
json::JSONValue txnexport::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;auto id=getParam("id");bool show=getBooleanParam("showtxn",false);auto row=db.getTransactionRow(getParam("id"));if(!row)throw CommandException("Transaction not found : "+id);
    json::JSONObject response;if(existsParam("file")){auto file=MiniFile::createBaseFile(getParam("file"));std::error_code error;if(std::filesystem::exists(file,error))std::filesystem::remove(file,error);MiniFile::writeObjectToFile(file,*row);response.put("file",std::filesystem::absolute(file).string());auto length=std::filesystem::file_size(file,error);response.put("size",MiniFormat::formatSize(error?0:std::bit_cast<int64_t>(uint64_t(length))));}
    else {auto bytes=MiniData(row->serialise());if(show)response.put("txn",row->toJSON());response.put("data",bytes.to0xString());}result.put("response",response);return result;
}
std::shared_ptr<Command> txnexport::getFunction() const {return std::make_shared<txnexport>();}
}
