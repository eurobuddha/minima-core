#include "rawfrom.hpp"
#include "support/RawTxnCommands.hpp"
namespace minima::commands {
rawfrom::rawfrom():Command("rawfrom","[inputs:] [outputs:] (state:) - [DEPRECATED USE rawtxnfrom ] Create an unsigned transaction from a set of inputs, outputs and state"){}
std::optional<std::string> rawfrom::getFullHelp() const {return std::string(
"\nrawfrom\n"
"\n"
"Create an unsigned transaction with a list of input and output JSON coins.\n"
"\n"
"inputs:\n"
"    A JSONArray of input JSON coins with a coinid and script value.\n"
"\n"
"outputs:\n"
"    A JSONArray of output JSON coins with an address, amount and optional tokenid and storestate.\n"
"\n"
"state: (optional)\n"
"    The JSON state.\n"
"\n"
"Examples:\n"
"\n"
"rawfrom inputs:[{\"coinid\":\"0x859EC55D1DC0DD66E72C1D8B6CDBD5E12D0EC5CB56255F629BAEC590047C22C4\",\"script\":\"RETURN SIGNEDBY(0x677228489AD4C14AC0D04F9BA054974A2B1D60C5A8E8D710CF9F2BF64D1EE81E)\"}] outputs:[{\"address\":\"0x6A3A060CE9D8E876E9B12DBE5D8F6A6864183BA1A63EA8CBCC14D668B5BECACF\",\"amount\":\"9.9996\",\"storestate\":false,\"tokenid\":\"0x00\"}] state:{\"0\":\"98\",\"1\":\"[MESSAGE]\"}\n");}
std::vector<std::string> rawfrom::getValidParams() const {return {"inputs","outputs","state"};}
std::shared_ptr<Command> rawfrom::getFunction() const {return std::make_shared<rawfrom>();}
json::JSONObject rawfrom::runCommand(const std::string& command){return command_result::first(command);}
json::JSONValue rawfrom::runCommand(){
    auto ret=getJSONReply();auto inputs=getJSONArrayParam("inputs");auto outputs=getJSONArrayParam("outputs");
    std::shared_ptr<json::JSONObject> state;if(existsParam("state"))state=getJSONObjectParam("state");
    auto id=MiniData::getRandomData(32).to0xString();auto result=runCommand("txncreate id:"+id);
    for(const auto& item:sqlRequired(inputs).elements()){
        const auto& input=raw_detail::object(item);auto coin=raw_detail::stringText(input.get("coinid"));auto script=raw_detail::stringText(input.get("script"));
        runCommand("txninput id:"+id+" coinid:"+coin);runCommand("txnscript id:"+id+" scripts:{\""+script+"\":\"\"}");
    }
    raw_detail::outputs(*this,id,outputs);
    raw_detail::state(*this,id,state);result=runCommand("txnexport id:"+id+" showtxn:true");runCommand("txndelete id:"+id);
    ret.put("response",result.get("response"));return ret;
}
}
