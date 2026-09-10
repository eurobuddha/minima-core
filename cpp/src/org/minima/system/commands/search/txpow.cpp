#include "txpow.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/brains/TxPoWSearcher.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/RawTxnCommands.hpp"
#include <unordered_set>
namespace minima::commands {
txpow::txpow():Command("txpow","(txpowid:) (onchain:) (block:) (address:) (relevant:) (max:) - Search for a specific TxPoW or check for onchain"){}
std::optional<std::string> txpow::getFullHelp() const {return std::string(
"\ntxpow\n"
"\n"
"Search for a specific TxPoW in the unpruned chain or your mempool.\n"
"\n"
"Search by txpowid, block or 0x / Mx address.\n"
"\n"
"txpowid: (optional)\n"
"    TxPoW id of the TxPoW to search for.\n"
"    Returns the txpow details.\n"
"\n"
"onchain: (optional)\n"
"    TxPoW id to search for on chain. Must be in the unpruned chain.\n"
"    Returns block info and number of confirmations.\n"
"\n"
"block: (optional)\n"
"    Block number to search in. Must be in the unpruned chain.\n"
"\n"
"address: (optional)\n"
"    0x or Mx address. Search for TxPoWs containing this specific address.\n"
"\n"
"relevant: (optional)\n"
"    true or false. Only list TxPoWs relevant to this node.\n"
"\n"
"max: (optional)\n"
"    Max relevant TxPoW to retrieve. Default 100.\n"
"\n"
"Examples:\n"
"\n"
"txpow txpowid:0x000..\n"
"\n"
"txpow block:200\n"
"\n"
"txpow address:0xCEF6..\n"
"\n"
"txpow onchain:0x000..\n");}
std::vector<std::string> txpow::getValidParams() const {return {"txpowid","block","address","onchain","relevant","max","action","inblock"};}
std::shared_ptr<Command> txpow::getFunction() const {return std::make_shared<txpow>();}
namespace {
json::JSONArray uniqueTxPoWs(const std::vector<std::shared_ptr<TxPoW>>& values){
    json::JSONArray result;std::unordered_set<std::string> seen;
    for(const auto& value:values){auto id=sqlRequired(value).getTxPoWID();if(seen.insert(id).second)result.add(value->toJSON());}
    return result;
}
std::string objectText(const json::JSONValue& value){
    if(value.isNull())throw JavaNullPointerException("null onchain field");return send_detail::stateText(value);
}
}
json::JSONValue txpow::runCommand(){
    auto ret=getJSONReply();
    auto db=[](){return sqlRequired(MinimaDB::getDB()).getTxPoWDB();};
    auto archive=[](){return sqlRequired(MinimaDB::getDB()).getArchive();};
    auto tip=[](){return sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWTree()).getTip();};
    if(existsParam("action")){
        auto action=getParam("action");
        if(action!="info")throw CommandException("Invalid action : "+action);
        json::JSONObject txdb;txdb.put("size",sqlRequired(db()).getSqlSize());
        auto onchain=sqlRequired(db()).getOnChainDB();json::JSONObject onchaindb;
        onchaindb.put("size",sqlRequired(onchain).getSize());onchaindb.put("first",onchain->getFirstTxPoW());onchaindb.put("last",onchain->getLastTxPoW());
        json::JSONObject info;info.put("storedays",GeneralParams::NUMBER_DAYS_SQLTXPOWDB);info.put("txpowdb",txdb);info.put("onchaindb",onchaindb);ret.put("response",info);
    }else if(existsParam("txpowid")){
        auto id=getAddressParam("txpowid");auto value=sqlRequired(db()).getTxPoW(id);
        if(!value){
            auto block=sqlRequired(archive()).loadBlock(id);
            if(block)value=block->getSharedTxPoW();
            else{
                bool autologin=sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getAutoLoginDetailsMySQL();
                if(autologin){
                    auto result=command_result::first("mysql action:findtxpow txpowid:"+id);const auto& response=raw_detail::object(result.get("response"));
                    if(command_result::booleanValue(response.get("found"))){ret.put("response",response.get("txpow"));return ret;}
                }
                throw CommandException("TxPoW not found : "+id);
            }
        }
        ret.put("response",sqlRequired(value).toJSON());
    }else if(existsParam("relevant")){
        auto max=getNumberParam("max",TxPoWSqlDB::MAX_RELEVANT_TXPOW).getAsInt();
        ret.put("response",uniqueTxPoWs(sqlRequired(sqlRequired(db()).getSQLDB()).getAllRelevant(max)));
    }else if(existsParam("inblock")){
        auto onchain=sqlRequired(db()).getOnChainDB();json::JSONObject response;json::JSONArray values;auto inblock=getParam("inblock");
        if(inblock.starts_with("0x")){auto id=getDataParam("inblock");values=sqlRequired(onchain).getInBlockTxPoW(id.to0xString());}
        else{auto block=getNumberParam("inblock");values=sqlRequired(onchain).getInBlockTxPoW(block.getAsLong());}
        response.put("txns",values);ret.put("response",response);
    }else if(existsParam("onchain")){
        auto id=getDataParam("onchain");json::JSONObject response;auto block=TxPoWSearcher::searchChainForTxPoW(id);
        if(!block){
            auto onchain=sqlRequired(db()).getOnChainDB();auto found=sqlRequired(onchain).getOnChainTxPoW(id.to0xString());
            if(command_result::booleanValue(found.get("found"))){
                MiniNumber height(objectText(found.get("block")));auto current=tip();response.put("found",true);response.put("block",height.toString());
                response.put("blockid",objectText(found.get("blockid")));response.put("tip",sqlRequired(current).getBlockNumber().toString());
                response.put("confirmations",current->getBlockNumber().sub(height).toString());
            }else response.put("found",false);
        }else{
            auto current=tip();response.put("found",true);response.put("block",block->getBlockNumber().toString());response.put("blockid",block->getTxPoWID());
            response.put("tip",sqlRequired(current).getBlockNumber().toString());response.put("confirmations",current->getBlockNumber().sub(block->getBlockNumber()).toString());
        }
        ret.put("response",response);
    }else if(existsParam("block")){
        auto height=getNumberParam("block");auto value=TxPoWSearcher::getTxPoWBlock(height);
        if(!value){auto block=sqlRequired(archive()).loadBlockFromNumber(height);if(!block)throw CommandException("TxPoW not found @ height "+height.toString());value=block->getSharedTxPoW();}
        ret.put("response",sqlRequired(value).toJSON());
    }else if(existsParam("address")){
        auto address=getAddressParam("address");ret.put("response",uniqueTxPoWs(TxPoWSearcher::searchTxPoWviaAddress(MiniData::fromHex(address))));
    }else throw CommandException("Must Specify search params");
    return ret;
}
}
