#include "createtokenfrom.hpp"
#include "support/CommandResult.hpp"
#include "support/JavaInteger.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include <bit>
namespace minima::commands {
createtokenfrom::createtokenfrom():Command("createtokenfrom","[fromaddress:] [script:] [privatekey:] [keyuses:] [name:] [amount:] (decimals:) (mine:)- Create Tokens from a certain address"){}
std::vector<std::string> createtokenfrom::getValidParams() const {return {"fromaddress","name","amount","privatekey","keyuses","script","decimals","mine"};}
std::shared_ptr<Command> createtokenfrom::getFunction() const {return std::make_shared<createtokenfrom>();}
json::JSONObject createtokenfrom::runCommand(const std::string& command){return command_result::first(command);}
json::JSONValue createtokenfrom::runCommand(){
    auto ret=getJSONReply();auto from=getAddressParam("fromaddress");auto amount=getNumberParam("amount");
    auto script=getParam("script");auto privatekey=getAddressParam("privatekey");auto keyuses=getNumberParam("keyuses");bool mine=getBooleanParam("mine",true);
    std::shared_ptr<json::JSONObject> name;
    if(isParamJSONObject("name")){
        name=getJSONObjectParam("name");if(!sqlRequired(name).containsKey("name"))throw CommandException("MUST specify a 'name' for the token in the JSON");
    }else{name=std::make_shared<json::JSONObject>();name->put("name",getParam("name"));}
    int32_t decimals=8;
    if(getParams()->containsKey("decimals")){
        const auto& value=getParams()->get("decimals");
        if(value.isNull())throw JavaNumberFormatException("Cannot parse null string");
        if(!value.isString())throw JavaClassCastException("Cannot cast decimals to String");
        decimals=javaParseInt(value.asString());if(decimals>16)throw JavaCheckedException("MAX 16 decimal places");
    }
    auto total=amount.floor();if(total.isMore(MiniNumber::TRILLION))throw CommandException("MAX 1 Trillion coins for a token");
    if(total.isLessEqual(MiniNumber::ZERO))throw CommandException("Cannot create less than 1 token");
    auto totaldecs=MiniNumber::TEN.pow(decimals);auto minimamount=MiniNumber::MINI_UNIT.mult(totaldecs).mult(total);
    auto scale=std::bit_cast<int32_t>(uint32_t(MiniNumber::MAX_DECIMAL_PLACES)-uint32_t(decimals));
    auto id=MiniData::getRandomData(32).to0xString();auto result=runCommand("txncreate id:"+id);
    result=runCommand("txnaddamount id:"+id+" fromaddress: "+from+" address:"+from+" amount:"+minimamount.toString());
    if(!command_result::status(result)){runCommand("txndelete id:"+id);command_result::reject(result);}
    Coin output(Coin::COINID_OUTPUT,MiniData::fromHex(from),minimamount,Token::TOKENID_CREATE,true);
    Token token(Coin::COINID_OUTPUT,MiniNumber(scale),minimamount,MiniString(name->toJSONString()),MiniString("RETURN TRUE"));output.setToken(token);
    auto db=sqlRequired(MinimaDB::getDB()).getCustomTxnDB();auto row=sqlRequired(db).getTransactionRow(id);auto transaction=sqlRequired(row).getTransaction();
    auto& outputs=sqlRequired(transaction).getAllOutputs();
    if(outputs.empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
    outputs[0]=output;
    runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");runCommand("txnmmr id:"+id);
    runCommand("txnsign id:"+id+" publickey:custom privatekey:"+privatekey+" keyuses:"+keyuses.toString());
    result=runCommand("txnpost id:"+id+" mine:"+(mine?"true":"false"));runCommand("txndelete id:"+id);
    if(!command_result::status(result))command_result::reject(result);
    ret.put("response",result.get("response"));return ret;
}
}
