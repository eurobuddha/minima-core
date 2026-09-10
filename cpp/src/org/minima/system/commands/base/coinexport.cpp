#include "coinexport.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/brains/TxPoWGenerator.hpp"
#include "org/minima/database/userprefs/txndb/TxnDB.hpp"
namespace minima::commands {
coinexport::coinexport():Command("coinexport","[coinid:] - Export a coin"){}
std::optional<std::string> coinexport::getFullHelp() const {return std::string(
"\ncoinexport\n"
"\n"
"Export a coin including its MMR proof.\n"
"\n"
"A coin can then be imported and tracked on another node using the 'coinimport' command.\n"
"\n"
"This does not allow the spending of a coin - just the knowledge of its existence.\n"
"\n"
"coinid:\n"
"    The id of a coin. Can be found using the 'coins' command.\n"
"\n"
"Examples:\n"
"\n"
"coinexport coinid:0xCD34..\n");}
std::vector<std::string> coinexport::getValidParams() const {return {"coinid"};}
json::JSONValue coinexport::runCommand(){
    auto result=getJSONReply();auto id=getParam("coinid");auto coin=TxPoWSearcher::searchCoin(MiniData::fromHex(id));if(!coin)throw CommandException("Coin not found coinid : "+id);
    auto tip=TxPoWSearcher::services.getTip();if(!tip)throw std::runtime_error("Null chain tip");auto back=tip->getBlockNumber().sub(MiniNumber(GeneralParams::TEST_PARAMS?8:256));if(back.isLess(coin->getBlockCreated()))back=coin->getBlockCreated();auto node=tip->getPastNode(back);if(!node)throw std::runtime_error("Missing MMR history node");auto proof=node->getMMR()->getProofToPeak(coin->getMMREntryNumber());CoinProof combined(*coin,proof);json::JSONObject response;response.put("coinproof",combined.toJSON());response.put("data",MiniData(combined.serialise()).to0xString());result.put("response",response);return result;
}
std::shared_ptr<Command> coinexport::getFunction() const {return std::make_shared<coinexport>();}
}
