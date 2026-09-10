#include "org/minima/system/commands/Command.hpp"
#include <iostream>
#include "org/minima/system/commands/base/debugflag.hpp"
#include "org/minima/system/commands/txn/txnlock.hpp"
#include "org/minima/system/commands/base/hash.hpp"
#include "org/minima/system/commands/scripts/runscript.hpp"
#include "org/minima/system/commands/signatures/verify.hpp"
#include "support/JavaInteger.hpp"
#include "org/minima/system/commands/base/maths.hpp"
#include "org/minima/system/commands/base/mmrcreate.hpp"
#include "org/minima/system/commands/base/mmrproof.hpp"

#include "org/minima/system/commands/base/missingcmd.hpp"
#include "org/minima/system/commands/base/convert.hpp"
#include "org/minima/system/commands/base/random.hpp"
#include "org/minima/system/commands/base/checkaddress.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
using namespace minima;
static auto string(const json::JSONValue& value)->std::optional<std::string>{return value.isNull()?std::nullopt:std::optional(value.asString());}
static json::JSONValue value(const std::optional<std::string>& s){return s?json::JSONValue(*s):json::JSONValue();}
template<class T>static json::JSONValue object(std::shared_ptr<T> p){json::JSONValue value;if(p)value.value=std::move(p);return value;}
class Test:public Command{public:using Command::Command;json::JSONValue runCommand()override{return getJSONReply();}std::shared_ptr<Command> getFunction()const override{return std::make_shared<Test>(getName(),getHelp());}};
int main(){
    TxPoWTreeNode::services.isAddressRelevant=[](const std::string& address){return address=="0x"+std::string(64,'1')||address=="0x"+std::string(64,'2');};
    TxPoWTreeNode::services.isAddressSimple=[](const std::string& address){return address=="0x"+std::string(64,'1')||address=="0x"+std::string(64,'3');};
    std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);auto op=data->get("op").asString();if(op=="integer"){std::cout<<javaParseInt(data->get("value").asString())<<'\n';continue;}if(op=="mx"){std::cout<<json::JSONValue(Address::makeMinimaAddress(MiniData::fromHex(data->get("value").asString()))).toJSONString()<<'\n';continue;}
    if(op=="command"||op=="commandmeta"){auto name=data->get("command").asString();std::shared_ptr<Command> cmd;if(name=="debugflag")cmd=std::make_shared<commands::debugflag>();else if(name=="txnlock")cmd=std::make_shared<commands::txnlock>();else if(name=="convert")cmd=std::make_shared<commands::convert>();else if(name=="random")cmd=std::make_shared<commands::random>();else if(name=="checkaddress")cmd=std::make_shared<commands::checkaddress>();else if(name=="maths")cmd=std::make_shared<commands::maths>();else if(name=="mmrcreate")cmd=std::make_shared<commands::mmrcreate>();else if(name=="mmrproof")cmd=std::make_shared<commands::mmrproof>();else if(name=="runscript")cmd=std::make_shared<commands::runscript>();else if(name=="verify")cmd=std::make_shared<commands::verify>();else throw std::invalid_argument(name);auto params=std::get<std::shared_ptr<json::JSONObject>>(data->get("params").value);for(const auto& [k,v]:params->entries())cmd->getParams()->put(k,v);if(op=="commandmeta"){json::JSONObject meta;meta.put("name",value(cmd->getName()));meta.put("help",value(cmd->getHelp()));meta.put("fullhelp",value(cmd->getFullHelp()));json::JSONArray valid;for(const auto& p:cmd->getValidParams())valid.add(p);meta.put("valid",valid);meta.put("fresh",cmd->getFunction()->getJSONReply());std::cout<<meta.toJSONString()<<'\n';}else std::cout<<cmd->runCommand().toJSONString()<<'\n';continue;}
    if(op=="hash"||op=="hashmeta"||op=="missing"){std::shared_ptr<Command> cmd;if(op=="missing")cmd=std::make_shared<commands::missingcmd>(string(data->get("input")),string(data->get("error")));else cmd=std::make_shared<commands::hash>();auto params=std::get<std::shared_ptr<json::JSONObject>>(data->get("params").value);for(const auto& [k,v]:params->entries())cmd->getParams()->put(k,v);if(op=="hashmeta"){json::JSONObject meta;meta.put("help",value(cmd->getFullHelp()));json::JSONArray valid;for(const auto& p:cmd->getValidParams())valid.add(p);meta.put("valid",valid);meta.put("fresh",cmd->getFunction()->getJSONReply());std::cout<<meta.toJSONString()<<'\n';}else std::cout<<cmd->runCommand().toJSONString()<<'\n';continue;}
    Test command(string(data->get("name")),string(data->get("help")));auto params=std::get<std::shared_ptr<json::JSONObject>>(data->get("params").value);for(const auto& [k,v]:params->entries())command.getParams()->put(k,v);command.setCompleteCommand(string(data->get("complete")));command.setMiniDAPPID(string(data->get("id")));auto key=data->get("key").asString();const auto& fallback=data->get("default");json::JSONValue result;
    if(op=="param")result=command.getParam(key);
    else if(op=="paramdefault")result=fallback.isNull()?value(command.getParam(key,nullptr)):json::JSONValue(command.getParam(key,fallback.asString()));
    else if(op=="boolean")result=command.getBooleanParam(key);else if(op=="booleandefault")result=command.getBooleanParam(key,fallback.asBool());
    else if(op=="number")result=command.getNumberParam(key).toString();else if(op=="numberdefault"){if(fallback.isNull()){auto n=command.getNumberParam(key,nullptr);if(n)result=n->toString();}else result=command.getNumberParam(key,MiniNumber(fallback.asString())).toString();}
    else if(op=="data")result=command.getDataParam(key).to0xString();else if(op=="datadefault"){if(fallback.isNull()){auto d=command.getDataParam(key,nullptr);if(d)result=d->to0xString();}else result=command.getDataParam(key,MiniData::fromHex(fallback.asString())).to0xString();}
    else if(op=="address")result=command.getAddressParam(key);else if(op=="addressdefault")result=fallback.isNull()?value(command.getAddressParam(key,nullptr)):json::JSONValue(command.getAddressParam(key,fallback.asString()));
    else if(op=="object")result=object(command.getJSONObjectParam(key));else if(op=="objectdefault")result=object(command.getJSONObjectParam(key,fallback.isNull()?nullptr:std::get<std::shared_ptr<json::JSONObject>>(fallback.value)));else if(op=="array")result=object(command.getJSONArrayParam(key));
    else if(op=="meta"){json::JSONObject meta;meta.put("name",value(command.getName()));meta.put("help",value(command.getHelp()));meta.put("fullhelp",value(command.getFullHelp()));meta.put("complete",value(command.getCompleteCommand()));meta.put("id",value(command.getMiniDAPPID()));meta.put("exists",command.existsParam(key));meta.put("object",command.isParamJSONObject(key));meta.put("array",command.isParamJSONArray(key));json::JSONArray valid;for(const auto& p:command.getValidParams())valid.add(p);meta.put("valid",valid);result=meta;}
    else if(op=="reply"){auto reply=command.getJSONReply();command.getParams()->put("later",true);result=reply;}
    else if(op=="alias"){auto nested=command.getJSONObjectParam(key);if(nested)nested->put("later",true);result=command.getJSONReply();}else throw std::invalid_argument("Unknown operation");std::cout<<result.toJSONString()<<'\n';
}catch(const CommandException& e){std::cout<<json::JSONValue(std::string("COMMAND_ERROR:")+e.what()).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"\"ERROR\"\n";}}
