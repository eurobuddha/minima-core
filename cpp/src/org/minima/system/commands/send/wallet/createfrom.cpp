#include "createfrom.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
createfrom::createfrom():Command("createfrom","[fromaddress:] [address:] [amount:] (tokenid:) [script:] (burn:) - Create unsigned txn from a certain address"){}
std::vector<std::string> createfrom::getValidParams() const {return {"fromaddress","address","amount","tokenid","script","burn"};}
std::shared_ptr<Command> createfrom::getFunction() const {return std::make_shared<createfrom>();}
json::JSONObject createfrom::runCommand(const std::string& command){
    return command_result::first(command);
}
json::JSONValue createfrom::runCommand(){
    auto ret=getJSONReply();auto from=getAddressParam("fromaddress");auto to=getAddressParam("address");
    auto amount=getNumberParam("amount");auto token=getAddressParam("tokenid","0x00");auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isMore(MiniNumber::ZERO)&&token!="0x00")throw CommandException("Currently BURN on precreated transactions only works for Minima.. tokenid:0x00.. not tokens.");
    auto script=getParam("script");auto id=MiniData::getRandomData(32).to0xString();
    auto result=runCommand("txncreate id:"+id);
    result=runCommand("txnaddamount id:"+id+" burn:"+burn.toString()+" fromaddress: "+from+" address:"+to+" amount:"+amount.toString()+" tokenid:"+token);
    if(!command_result::status(result)){runCommand("txndelete id:"+id);command_result::reject(result);}
    runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");
    runCommand("txnmmr id:"+id);result=runCommand("txnexport id:"+id+" showtxn:true");runCommand("txndelete id:"+id);
    ret.put("response",result.get("response"));return ret;
}
}
