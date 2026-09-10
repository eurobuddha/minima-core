#include "coinimport.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
namespace minima::commands {
coinimport::coinimport():Command("coinimport","[data:] (track:true|false) - Import a coin, and keep tracking it"){}
std::optional<std::string> coinimport::getFullHelp() const {return std::string(
"\ncoinimport\n"
"\n"
"Import a coin including its MMR proof.\n"
"\n"
"Optionally you can track the coin to add it to your relevant coins list and know when it becomes spent.\n"
"\n"
"Importing does not allow the spending of a coin - just the knowledge of its existence.\n"
"\n"
"data:\n"
"    The data of a coin. Can be found using the 'coinexport' command.\n"
"\n"
"track: (optional)\n"
"    true or false, true will create an MMR entry for the coin and add it to your relevant coins.\n"
"\n"
"Examples:\n"
"\n"
"coinimport data:0x00000..\n");}
json::JSONValue coinimport::runCommand(){
    auto result=getJSONReply();auto data=getParam("data");
    auto proof=CoinProof::convertMiniDataVersion(MiniData::fromHex(data));
    if(!proof)throw JavaNullPointerException("null imported coin proof");
    auto coin=proof->getCoin();
    if(coin.getSpent())throw CommandException("Coin is spent. Can only import UNSPENT coins.");
    auto tip=sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();
    auto mmr=sqlRequired(tip).getMMR();
    CoinProof current(coin,sqlRequired(mmr).getProofToPeak(coin.getMMREntryNumber()));
    auto leaf=MMRData::CreateMMRDataLeafNode(current.getCoin(),current.getCoin().getAmount());
    if(mmr->checkProofTimeValid(coin.getMMREntryNumber(),leaf,current.getMMRProof())){
        auto node=TxPoWSearcher::getTreeNodeForCoin(coin.getCoinID());
        if(node){
            if(node->isRelevantEntry(coin.getMMREntryNumber()))throw CommandException("Attempting to add relevant coin we already have");
            node->getRelevantCoinsEntries().push_back(coin.getMMREntryNumber());node->calculateRelevantCoins();
            result.put("response",proof->toJSON());return result;
        }
    }
    leaf=MMRData::CreateMMRDataLeafNode(proof->getCoin(),proof->getCoin().getAmount());
    if(!mmr->checkProofTimeValid(coin.getMMREntryNumber(),leaf,proof->getMMRProof()))throw CommandException("Invalid MMR Proof");
    auto block=proof->getMMRProof().getBlockTime();auto node=tip->getPastNode(block);
    if(!node)throw CommandException("TreeNode at Blocktime not found (proof too old): "+block.toString());
    auto historical=node->getMMR();auto oldRoot=sqlRequired(historical).getRoot();
    auto mmrdata=proof->getMMRData();(void)mmrdata;
    historical->setFinalized(false);historical->updateEntry(coin.getMMREntryNumber(),proof->getMMRProof(),proof->getMMRData());historical->finalizeSet();
    node->addCoinToNode(coin);
    if(getBooleanParam("track",true)){node->getRelevantCoins().push_back(coin);node->getRelevantCoinsEntries().push_back(coin.getMMREntryNumber());}
    auto newRoot=historical->getRoot();
    if(!sqlRequired(newRoot).isEqual(sqlRequired(oldRoot)))throw CommandException("SERIOUS ERROR : MMR root different after adding coin.. "+proof->toJSON().toString());
    result.put("response",proof->toJSON());return result;
}
}
