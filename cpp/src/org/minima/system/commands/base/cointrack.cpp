#include "cointrack.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
namespace minima::commands {
cointrack::cointrack():Command("cointrack","[enable:true|false] [coinid:] - Track or untrack a coin"){}
std::optional<std::string> cointrack::getFullHelp() const {return std::string(
"\ncointrack\n"
"\n"
"Track or untrack a coin.\n"
"\n"
"Track a coin to keep its MMR proof up-to-date and know when it becomes spent. Stop tracking to remove it from your relevant coins list.\n"
"\n"
"enable:\n"
"    true or false, true will add the coin to your relevant coins, false will remove it from your relevant coins.\n"
"\n"
"coinid:\n"
"    The id of a coin. Can be found using the 'coins' command.\n"
"\n"
"Examples:\n"
"\n"
"cointrack enable:true coinid:0xCD34..\n");}
std::vector<std::string> cointrack::getValidParams() const {return {"enable","coinid"};}
std::shared_ptr<Command> cointrack::getFunction() const {return std::make_shared<cointrack>();}
json::JSONValue cointrack::runCommand(){
    auto ret=getJSONReply();auto id=getParam("coinid");auto data=MiniData::fromHex(id);bool track=getBooleanParam("enable");
    auto node=TxPoWSearcher::getTreeNodeForCoin(MiniData::fromHex(id));if(!node)throw CommandException("Coin not found coinid : "+id);
    auto coin=TxPoWSearcher::searchCoin(data,false);if(!coin)throw JavaNullPointerException("null tracked coin");
    if(track){
        if(node->isRelevantEntry(coin->getMMREntryNumber()))ret.put("response","Coin already tracked");
        else{node->getRelevantCoinsEntries().push_back(coin->getMMREntryNumber());node->calculateRelevantCoins();ret.put("response","Coin added to track list");}
    }else{node->removeRelevantCoin(coin->getMMREntryNumber());node->calculateRelevantCoins();ret.put("response","Coin removed from track list");}
    return ret;
}
}
