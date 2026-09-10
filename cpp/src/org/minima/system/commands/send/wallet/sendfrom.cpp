#include "sendfrom.hpp"
#include "support/RawTxnCommands.hpp"
namespace minima::commands {
sendfrom::sendfrom():Command("sendfrom","[fromaddress:] [address:] [amount:] [script:] [privatekey:] [keyuses:] (tokenid:) (state:) (split:) (burn:) (mine:) - Send Minima or Tokens from a certain address"){}
std::vector<std::string> sendfrom::getValidParams() const {return {"fromaddress","address","amount","tokenid","script","privatekey","keyuses","mine","burn","state","split"};}
std::shared_ptr<Command> sendfrom::getFunction() const {return std::make_shared<sendfrom>();}
json::JSONObject sendfrom::runCommand(const std::string& command){return command_result::first(command);}
json::JSONValue sendfrom::runCommand(){
    auto ret=getJSONReply();auto from=getAddressParam("fromaddress");auto to=getAddressParam("address");
    auto amount=getNumberParam("amount");auto token=getAddressParam("tokenid","0x00");auto burn=getNumberParam("burn",MiniNumber::ZERO);
    if(burn.isMore(MiniNumber::ZERO)&&token!="0x00")throw CommandException("Currently BURN on precreated transactions only works for Minima.. tokenid:0x00.. not tokens.");
    auto script=getParam("script");auto privatekey=getAddressParam("privatekey");auto keyuses=getNumberParam("keyuses");
    auto id=MiniData::getRandomData(32).to0xString();bool mine=getBooleanParam("mine",true);auto split=getNumberParam("split",MiniNumber::ONE);
    std::shared_ptr<json::JSONObject> state;
    if(existsParam("state"))try{state=getJSONObjectParam("state");}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){state=nullptr;}
    auto result=runCommand("txncreate id:"+id);
    result=runCommand("txnaddamount id:"+id+" split:"+split.toString()+" burn:"+burn.toString()+" fromaddress: "+from+" address:"+to+" amount:"+amount.toString()+" tokenid:"+token);
    if(!command_result::status(result)){runCommand("txndelete id:"+id);command_result::reject(result);}
    runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");
    raw_detail::state(*this,id,state);runCommand("txnmmr id:"+id);
    runCommand("txnsign id:"+id+" publickey:custom privatekey:"+privatekey+" keyuses:"+keyuses.toString());
    result=runCommand("txnpost id:"+id+" mine:"+(mine?"true":"false"));runCommand("txndelete id:"+id);
    if(!command_result::status(result))command_result::reject(result);
    ret.put("response",result.get("response"));return ret;
}
}
