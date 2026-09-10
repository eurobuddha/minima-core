#include "postfrom.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
postfrom::postfrom():Command("postfrom","[data:] (mine:true|false) (mmr:true|false) - Post a signfrom txn "){}
std::vector<std::string> postfrom::getValidParams() const {return {"data","mine","mmr"};}
std::shared_ptr<Command> postfrom::getFunction() const {return std::make_shared<postfrom>();}
json::JSONObject postfrom::runCommand(const std::string& command){
    return command_result::first(command);
}
json::JSONValue postfrom::runCommand(){
    auto ret=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto data=getDataParam("data");auto row=TxnRow::convertMiniDataVersion(data);
    if(existsParam("id")){auto id=getParam("id");sqlRequired(row).setID(id);}auto mmr=getBooleanParam("mmr",false);auto id=sqlRequired(row).getID().value_or("null");
    sqlRequired(db).addCompleteTransaction(row);auto mine=getBooleanParam("mine",false);
    if(mmr)runCommand("txnmmr id:"+id);auto result=runCommand("txnpost id:"+id+" mine:"+(mine?"true":"false"));
    runCommand("txndelete id:"+id);ret.put("response",result.get("response"));return ret;
}
}
