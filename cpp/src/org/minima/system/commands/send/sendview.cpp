#include "sendview.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "support/SqlRow.hpp"
namespace minima::commands {
sendview::sendview():Command("sendview","[file:] - View a transaction ( signed or unsigned )"){}
std::optional<std::string> sendview::getFullHelp() const {return std::string(
"\nsendview\n"
"\n"
"View a transaction ( signed or unsigned ).\n"
"\n"
"View the details of a txn created by the 'sendnosign' command by specifying its .txn file.\n"
"\n"
"file:\n"
"    Name of the transaction (.txn) file to view, located in the node's base folder.\n"
"    If not in the base folder, specify the full file path.\n"
"\n"
"Examples:\n"
"\n"
"sendview file:unsignedtransaction-1674907380057.txn\n"
"\n"
"sendview file:C:\\Users\\signedtransaction-1674907380057.txn\n"
"\n");}
std::vector<std::string> sendview::getValidParams() const {return {"file"};}
std::shared_ptr<Command> sendview::getFunction() const {return std::make_shared<sendview>();}
json::JSONValue sendview::runCommand(){
    auto result=getJSONReply();auto filename=getParam("file");
    auto bytes=MiniFile::readCompleteFile(MiniFile::createBaseFile(filename));
    auto pow=TxPoW::convertMiniDataVersion(MiniData(std::move(bytes)));
    if(!pow)throw JavaNullPointerException("null decoded TxPoW");
    json::JSONObject response;response.put("txpow",pow->toJSON());result.put("response",response);return result;
}
}
