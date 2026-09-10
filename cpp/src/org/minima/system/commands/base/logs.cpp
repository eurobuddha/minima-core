#include "logs.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp"
namespace minima::commands {
logs::logs():Command("logs","(scripts:) (mining:) (maxima:) (networking:) (blocks:) (ibd:) (txpowdb:) - Enable full logs for various parts of Minima"){}
std::optional<std::string> logs::getFullHelp() const {return "\nlogs\n\nEnable detailed logs for script errors or mining activity.\n\nscripts: (optional)\n    true or false, true turns on detailed logs for script errors.\n\nmining: (optional)\n    true or false, true turns on detailed logs for mining start/end activity.\n\nmaxima: (optional)\n    true or false, true turns on detailed logs for Maxima.\n\nnetworking: (optional)\n    true or false, true turns on detailed logs for Network Messages.\n\nblocks: (optional)\n    true or false, true turns on detailed logs for Blocks.\n\nibd: (optional)\n    true or false, true turns on detailed logs for IBD processing.\n\npeerschecker: (optional)\n    true or false, true turns on detailed logs for IBD processing.\n\ntxpowdb: (optional)\n    true or false, true turns on logs for adding TxPoW to TxPoWDB.\n\nExamples:\n\nlogs scripts:true\n\nlogs scripts:false mining:true\n";}
json::JSONValue logs::runCommand(){
    auto reply=getJSONReply();
    if(existsParam("scripts"))GeneralParams::SCRIPTLOGS=getParam("scripts","false")=="true";
    if(existsParam("mining"))GeneralParams::MINING_LOGS=getParam("mining","false")=="true";
    if(existsParam("maxima"))GeneralParams::MAXIMA_LOGS=getParam("maxima","false")=="true";
    if(existsParam("blocks"))GeneralParams::BLOCK_LOGS=getParam("blocks","false")=="true";
    if(existsParam("networking"))GeneralParams::NETWORKING_LOGS=getParam("networking","false")=="true";
    if(existsParam("ibd"))GeneralParams::IBDSYNC_LOGS=getParam("ibd","false")=="true";
    if(existsParam("peerschecker"))GeneralParams::PEERSCHECKER_lOG=getParam("peerschecker","false")=="true";
    if(existsParam("txpowdb")){bool enabled=getParam("txpowdb","false")=="true";TxPoWSqlDB::LOG_ADD_BLOCKS=enabled;TxPoWSqlDB::LOG_ADD_TRANSACTION=enabled;}
    json::JSONObject response;
    response.put("scripts",GeneralParams::SCRIPTLOGS.load());
    response.put("mining",GeneralParams::MINING_LOGS.load());
    response.put("maxima",GeneralParams::MAXIMA_LOGS.load());
    response.put("blocks",GeneralParams::BLOCK_LOGS.load());
    response.put("txpowdb",TxPoWSqlDB::LOG_ADD_BLOCKS.load());
    response.put("networking",GeneralParams::NETWORKING_LOGS.load());
    response.put("ibd",GeneralParams::IBDSYNC_LOGS.load());
    response.put("peerschecker",GeneralParams::PEERSCHECKER_lOG.load());
    reply.put("response",response);return reply;
}
}
