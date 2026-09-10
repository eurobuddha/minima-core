#include "constructfrom.hpp"
#include "support/CommandResult.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
constructfrom::constructfrom():Command("constructfrom","[coinlist:] [script:] [toaddress:] [toamount:] [changeaddress:] [changeamount:] (tokenid:) - Create unsigned txn from a list of coins"){}
std::vector<std::string> constructfrom::getValidParams() const {return {"coinlist","script","toaddress","toamount","changeaddress","changeamount","tokenid"};}
std::shared_ptr<Command> constructfrom::getFunction() const {return std::make_shared<constructfrom>();}
json::JSONObject constructfrom::runCommand(const std::string& command){
    return command_result::first(command);
}
json::JSONValue constructfrom::runCommand(){
    auto ret=getJSONReply();auto coins=getParam("coinlist");auto script=getParam("script");
    auto to=getAddressParam("toaddress");auto amount=getNumberParam("toamount");auto change=getNumberParam("changeamount");
    auto changeAddress=getAddressParam("changeaddress");auto token=getParam("tokenid","0x00");auto id=MiniData::getRandomData(32).to0xString();
    auto result=runCommand("txncreate id:"+id);
    size_t offset=0;
    while((offset=coins.find_first_not_of(',',offset))!=std::string::npos){
        auto end=coins.find(',',offset);auto coin=coins.substr(offset,end==std::string::npos?end:end-offset);
        offset=end==std::string::npos?coins.size():end;
        // StringTokenizer skips empty comma fields; String.trim removes <= U+0020.
        size_t begin=0,last=coin.size();while(begin<last&&static_cast<unsigned char>(coin[begin])<=32)++begin;while(last>begin&&static_cast<unsigned char>(coin[last-1])<=32)--last;
        coin=coin.substr(begin,last-begin);
        result=runCommand("txninput id:"+id+" coinid:"+coin);
        if(!command_result::status(result)){runCommand("txndelete id:"+id);command_result::reject(result);}
    }
    result=runCommand("txnoutput id:"+id+" address:"+to+" amount:"+amount.toString()+" tokenid:"+token);
    if(change.isMore(MiniNumber::ZERO))result=runCommand("txnoutput id:"+id+" address:"+changeAddress+" amount:"+change.toString()+" tokenid:"+token);
    runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");runCommand("txnmmr id:"+id);
    result=runCommand("txnexport id:"+id+" showtxn:true");runCommand("txndelete id:"+id);ret.put("response",result.get("response"));return ret;
}
}
