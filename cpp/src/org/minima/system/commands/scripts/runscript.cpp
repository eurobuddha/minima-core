#include "runscript.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaUpperCase.hpp"
namespace minima::commands {
runscript::runscript():Command("runscript","[script:] (state:{}) (prevstate:{}) (globals:{}) (signatures:[]) (extrascripts:{}) - Run a script with the defined parameters"){}
std::optional<std::string> runscript::getFullHelp() const {return std::string(
"\nrunscript\n"
"\n"
"Test run a script with predefined parameters without executing on chain.\n"
"\n"
"Scripts will be auto cleaned for you.\n"
"\n"
"script:\n"
"    The script to run, surrounded by double quotes.\n"
"\n"
"state: (optional)\n"
"    State variable values to use when running the script.\n"
"    JSON object in the format {0:value,1:value,..}.\n"
"\n"
"prevstate: (optional)\n"
"    The previous state variable values (for the input coin) to use when running the script.\n"
"    JSON object in the format {0:value,1:value,..}.\n"
"\n"
"globals: (optional)\n"
"    The Global variable values to use when running the script.\n"
"    JSON object in the format {@GLOBAL:value,..}.\n"
"\n"
"signatures: (optional)\n"
"    The signatures required for the script. JSON array.\n"
"\n"
"extrascripts: (optional)\n"
"    Extra scripts required for MAST contracts. \n"
"    JSON object in the format {script:proof,..}.\n"
"\n"
"Examples:\n"
"\n"
"runscript script:\"RETURN SIGNEDBY(0xFF..) AND @BLOCK GT 100\" globals:{\"@BLOCK\":\"101\"} signatures:[\"0xFF\"]\n"
"\n"
"runscript script:\"LET st=STATE(99) LET ps=PREVSTATE(99) IF st EQ ps AND @COINAGE GT 20\n"
"AND SIGNEDBY(0xFF) THEN RETURN TRUE ELSEIF st GT ps AND SIGNEDBY(0xEE) THEN RETURN TRUE ENDIF\"\n"
"globals:{\"@COINAGE\":\"23\"} state:{\"99\":\"0\"} prevstate:{\"99\":\"0\"} signatures:[\"0xFF\"]\n"
"\n"
"runscript script:\"MAST 0x0E3..\" extrascripts:{\"RETURN TRUE\":\"0x000..\"}\n");}
std::vector<std::string> runscript::getValidParams() const {return {"script","state","prevstate","globals","signatures","extrascripts"};}
json::JSONValue runscript::runCommand(){
    auto result=getJSONReply();auto script=getParam("script",std::string(""));
    auto state=getJSONObjectParam("state",std::make_shared<json::JSONObject>()),previous=getJSONObjectParam("prevstate",std::make_shared<json::JSONObject>()),globals=getJSONObjectParam("globals",std::make_shared<json::JSONObject>()),extra=getJSONObjectParam("extrascripts",std::make_shared<json::JSONObject>());
    auto signatures=existsParam("signatures")?getJSONArrayParam("signatures"):std::make_shared<json::JSONArray>();
    auto transaction=std::make_shared<Transaction>();auto witness=std::make_shared<Witness>();
    if(!state||!previous||!signatures||!extra)throw std::runtime_error("Null script context");
    for(const auto& [port,value]:state->entries())transaction->addStateVariable(StateVariable(javaParseInt(unicode::encodeUTF8(port)),value.asString()));
    std::vector<StateVariable> prevstate;for(const auto& [port,value]:previous->entries())prevstate.emplace_back(javaParseInt(unicode::encodeUTF8(port)),value.asString());
    std::vector<MiniData> keys;for(const auto& key:signatures->elements())keys.push_back(MiniData::fromHex(key.asString()));
    for(const auto& [code,proof]:extra->entries())witness->addScript(ScriptProof(unicode::encodeUTF8(code),MMRProof::convertMiniDataVersion(MiniData::fromHex(proof.asString()))));
    kissvm::Contract contract(script,keys,witness,transaction,prevstate);contract.setGlobalVariable("@SCRIPT",std::make_shared<kissvm::StringValue>(script));
    if(!globals)throw std::runtime_error("Null script globals");
    if(globals->containsKey("@BLOCK")&&globals->containsKey("@CREATED")&&!globals->containsKey("@COINAGE")){
        auto block=MiniNumber(globals->get("@BLOCK").asString()),created=MiniNumber(globals->get("@CREATED").asString());globals->put("@COINAGE",block.sub(created).toString());
    }
    for(const auto& [name,value]:globals->entries())contract.setGlobalVariable(javaUpperCase(unicode::encodeUTF8(name)),kissvm::Value::getValue(value.asString()));
    contract.run();json::JSONObject response;
    auto scriptInfo=[](const std::string& text){json::JSONObject info;auto address=Address(text).getAddressData();info.put("script",text);info.put("address",address.to0xString());info.put("mxaddress",Address::makeMinimaAddress(address));return info;};
    response.put("script",scriptInfo(script));response.put("clean",scriptInfo(kissvm::Contract::cleanScript(script)));response.put("trace",contract.getCompleteTraceLog());response.put("variables",contract.getAllVariables());response.put("parseok",contract.isParseOK());response.put("monotonic",contract.isMonotonic());response.put("success",contract.isSuccess());result.put("response",response);return result;
}
std::shared_ptr<Command> runscript::getFunction() const {return std::make_shared<runscript>();}
}
