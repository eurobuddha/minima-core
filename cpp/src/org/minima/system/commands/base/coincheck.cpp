#include "coincheck.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
namespace minima::commands {
coincheck::coincheck():Command("coincheck","[data:] - Check a coin exists"){}
std::optional<std::string> coincheck::getFullHelp() const {return std::string(
"\ncoincheck\n"
"\n"
"Check a coin exists and is valid. Can only check unspent coins.\n"
"\n"
"Returns the coin details and whether the MMR proof is valid.\n"
"\n"
"data:\n"
"    The data of a coin. Can be found using the 'coinexport' command.\n"
"\n"
"Examples:\n"
"\n"
"coincheck data:0x00000..\n");}
std::vector<std::string> coincheck::getValidParams() const {return {"data"};}
json::JSONValue coincheck::runCommand(){
    auto result=getJSONReply();auto proof=CoinProof::convertMiniDataVersion(MiniData::fromHex(getParam("data")));if(!proof)throw std::runtime_error("Null decoded coin proof");const auto& coin=proof->getCoin();if(coin.getSpent())throw CommandException("Coin is spent. Can only check UNSPENT coins.");
    auto tip=TxPoWSearcher::services.getTip();auto leaf=MMRData::CreateMMRDataLeafNode(coin,coin.getAmount());if(!tip)throw std::runtime_error("Null chain tip");auto valid=tip->getMMR()->checkProofTimeValid(coin.getMMREntryNumber(),leaf,proof->getMMRProof());json::JSONObject response;response.put("proofblock",json::JSONValue::raw(proof->getMMRProof().getBlockTime().toString()));response.put("coin",coin.toJSON());response.put("valid",valid);result.put("response",response);return result;
}
std::shared_ptr<Command> coincheck::getFunction() const {return std::make_shared<coincheck>();}
}
