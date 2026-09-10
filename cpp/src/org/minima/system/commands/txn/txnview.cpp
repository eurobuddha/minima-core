#include "txnview.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "support/JavaInteger.hpp"
#include <bit>
namespace minima::commands {
txnview::txnview():Command("txnview","(file:) (data:) - View a transaction as a JSON."){}
std::optional<std::string> txnview::getFullHelp() const {return std::string(
"\ntxnimport\n"
"\n"
"View a transaction from previously exported HEX data or a .txn file.\n"
"\n"
"file: (optional)\n"
"    File name/path to the previously exported .txn file.\n"
"\n"
"data: (optional)\n"
"    HEX data of the previously exported transaction.\n"
"\n"
"Examples:\n"
"\n"
"txnview data:0x0000..\n"
"\n"
"txnview file:multisig.txn\n");}
std::vector<std::string> txnview::getValidParams() const {return {"file","data"};}
json::JSONValue txnview::runCommand(){
    auto result=getJSONReply();std::shared_ptr<TxnRow> row;
    if(existsParam("file")){auto file=MiniFile::createBaseFile(getParam("file"));std::error_code error;if(!std::filesystem::exists(file,error))throw CommandException("File does not exist : "+std::filesystem::absolute(file).string());row=TxnRow::convertMiniDataVersion(MiniData(MiniFile::readCompleteFile(file)));if(!row)throw std::runtime_error("Null decoded transaction");}
    else if(existsParam("data")){row=TxnRow::convertMiniDataVersion(getDataParam("data"));if(!row)throw std::runtime_error("Null decoded transaction");if(existsParam("id"))row->setID(getParam("id"));}else throw CommandException("Must specify file or data");
    result.put("response",row->toJSON());return result;
}
std::shared_ptr<Command> txnview::getFunction() const {return std::make_shared<txnview>();}
}
