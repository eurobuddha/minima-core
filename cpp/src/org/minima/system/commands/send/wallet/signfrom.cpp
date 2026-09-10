#include "signfrom.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
signfrom::signfrom():Command("signfrom","[data:] [privatekey:] [keyuses:] (post:) - Sign a creatfrom txn"){}
std::vector<std::string> signfrom::getValidParams() const {return {"id","data","privatekey","keyuses","post"};}
std::shared_ptr<Command> signfrom::getFunction() const {return std::make_shared<signfrom>();}
json::JSONObject signfrom::runCommand(const std::string& command){
    return command_result::first(command);
}
json::JSONValue signfrom::runCommand(){
    auto ret=getJSONReply();auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto data=getDataParam("data");auto row=TxnRow::convertMiniDataVersion(data);
    if(existsParam("id")){auto id=getParam("id");sqlRequired(row).setID(id);}auto id=sqlRequired(row).getID().value_or("null");sqlRequired(db).addCompleteTransaction(row);
    auto key=getAddressParam("privatekey");auto uses=getNumberParam("keyuses");auto post=getBooleanParam("post",false);
    auto signedResult=runCommand("txnsign id:"+id+" publickey:custom privatekey:"+key+" keyuses:"+uses.toString()+(post?" txnpostauto:true txnpostmine:true":""));
    auto result=runCommand("txnexport id:"+id);runCommand("txndelete id:"+id);
    auto response=result.get("response");
    if(response.isNull())throw JavaNullPointerException("null export response");
    auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&response.value);if(!object)throw JavaClassCastException("Cannot cast export response to JSONObject");
    sqlRequired(*object).put("post",post);
    if(post)try{
        auto value=signedResult.get("response");if(value.isNull())throw JavaNullPointerException("null signing response");
        auto signedObject=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value);if(!signedObject)throw JavaClassCastException("Cannot cast signing response to JSONObject");
        (*object)->put("txpow",sqlRequired(*signedObject).get("txpow"));
    }catch(const std::exception& error){auto throwable=std::make_shared<NativeThrowable>(error);if(throwable->kind()==MinimaLogger::Throwable::Kind::Error||throwable->kind()==MinimaLogger::Throwable::Kind::OutOfMemory)throw;MinimaLogger::log(throwable);}
    ret.put("response",response);return ret;
}
}
