#include "sendpost.hpp"
#include "../txn/txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "support/SqlRow.hpp"
namespace minima::commands {
sendpost::sendpost():Command("sendpost","[file:] - Post a signed txn"){}
std::optional<std::string> sendpost::getFullHelp() const {return std::string(
"\nsendpost\n"
"\n"
"Post a transaction previously created and signed using the 'sendnosign' and 'sendsign' commands.\n"
"\n"
"Must be posted from an online node within approximately 24 hours of creating to ensure MMR proofs are valid.\n"
"\n"
"file:\n"
"    Name of the signed transaction (.txn) file to post, located in the node's base folder.\n"
"    If not in the base folder, specify the full file path.\n"
"\n"
"Examples:\n"
"\n"
"sendpost file:signedtransaction-1674907380057.txn\n"
"\n"
"sendpost file:C:\\Users\\signedtransaction-1674907380057.txn\n"
"\n");}
std::vector<std::string> sendpost::getValidParams() const {return {"file"};}
std::shared_ptr<Command> sendpost::getFunction() const {return std::make_shared<sendpost>();}
json::JSONValue sendpost::runCommand(){
    auto result=getJSONReply();auto filename=getParam("file");
    auto bytes=MiniFile::readCompleteFile(MiniFile::createBaseFile(filename));
    auto decoded=TxPoW::convertMiniDataVersion(MiniData(std::move(bytes)));
    if(!decoded)throw JavaNullPointerException("null decoded TxPoW");
    auto pow=std::make_shared<TxPoW>(std::move(*decoded));pow->calculateTXPOWID();
    json::JSONObject response;response.put("txpow",pow->toJSON());result.put("response",response);
    sqlRequired(txnutils::getMiner()).mineTxPoWAsync(pow);return result;
}
}
