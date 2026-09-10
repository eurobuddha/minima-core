#include "txnimport.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnimport::txnimport():Command("txnimport","(id:) (file:) (data:) - Import a transaction as a file or HEX data. Optionally specify the ID"){}
std::optional<std::string> txnimport::getFullHelp() const {return std::string(
"\ntxnimport\n"
"\n"
"Import a transaction from previously exported HEX data or a .txn file.\n"
"\n"
"Optionally specify an ID to use for the new transaction.\n"
"\n"
"id: (optional)\n"
"    Choose an ID for the transaction you are importing.\n"
"\n"
"file: (optional)\n"
"    File name/path to the previously exported .txn file.\n"
"\n"
"data: (optional)\n"
"    HEX data of the previously exported transaction.\n"
"\n"
"Examples:\n"
"\n"
"txnimport data:0x0000..\n"
"\n"
"txnimport id:simpletxn data:0x0000..\n"
"\n"
"txnimport id:multisig file:multisig.txn\n");}
std::vector<std::string> txnimport::getValidParams() const {return {"id","file","data"};}
json::JSONValue txnimport::runCommand(){
    auto result=getJSONReply();auto owner=TxnDB::getRuntimeDB();auto& db=*owner;std::shared_ptr<TxnRow> row;
    if(existsParam("file")){auto file=MiniFile::createBaseFile(getParam("file"));std::error_code error;if(!std::filesystem::exists(file,error))throw CommandException("File does not exist : "+std::filesystem::absolute(file).string());row=TxnRow::convertMiniDataVersion(MiniData(MiniFile::readCompleteFile(file)));}
    else if(existsParam("data"))row=TxnRow::convertMiniDataVersion(getDataParam("data"));else throw CommandException("Must specify file or data");
    if(!row)throw std::runtime_error("Null decoded transaction");if(existsParam("id"))row->setID(getParam("id"));db.addCompleteTransaction(row);result.put("response",row->toJSON());return result;
}
std::shared_ptr<Command> txnimport::getFunction() const {return std::make_shared<txnimport>();}
}
