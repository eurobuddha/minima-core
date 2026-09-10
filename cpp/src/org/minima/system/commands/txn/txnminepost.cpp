#include "txnminepost.hpp"
#include "txnutils.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
namespace minima::commands {
txnminepost::txnminepost():Command("txnminepost","[data:] - Post a pre-mined transaction"){}
std::vector<std::string> txnminepost::getValidParams() const {return {"data"};}
std::shared_ptr<Command> txnminepost::getFunction() const {return std::make_shared<txnminepost>();}
std::function<void(std::shared_ptr<Message>)> txnminepost::postMessage=[](auto){throw JavaNullPointerException("null Main receiver");};
json::JSONValue txnminepost::runCommand() {
    auto result=getJSONReply();auto data=getDataParam("data");auto decoded=TxPoW::convertMiniDataVersion(data);
    auto txpow=decoded?std::make_shared<TxPoW>(std::move(*decoded)):nullptr;
    auto message=std::make_shared<Message>("MAIN_TXPOWMINED");message->addObject("txpow",MessageObject(txpow));postMessage(message);
    json::JSONObject response;response.put("data",sqlRequired(txpow).toJSON());result.put("response",response);return result;
}
}
