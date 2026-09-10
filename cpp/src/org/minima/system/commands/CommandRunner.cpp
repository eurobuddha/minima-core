#include "CommandRunner.hpp"
#include "network/disconnect.hpp"
#include "network/ping.hpp"
#include "network/network.hpp"
#include "network/message.hpp"
#include "network/p2pstate.hpp"

#include "base/quit.hpp"
#include "network/rpc.hpp"
#include "base/status.hpp"
#include "base/logs.hpp"
#include "base/healthcheck.hpp"
#include "base/trace.hpp"
#include "base/timemilli.hpp"
#include "base/hashtest.hpp"
#include "backup/rescue.hpp"
#include "network/connect.hpp"
#include "network/webhooks.hpp"
#include "base/hash.hpp"
#include "base/automine.hpp"
#include "base/block.hpp"
#include "base/printmmr.hpp"
#include "base/printtree.hpp"
#include "base/newaddress.hpp"
#include "base/getaddress.hpp"
#include "base/coinnotify.hpp"
#include "base/slavenode.hpp"

#include "base/debugflag.hpp"
#include "txn/txnlock.hpp"
#include "txn/txnoutput.hpp"
#include "txn/txnmmr.hpp"
#include "txn/txninput.hpp"
#include "txn/txnbasics.hpp"
#include "txn/txncheck.hpp"
#include "txn/txnpost.hpp"
#include "txn/txnsign.hpp"
#include "backup/vault.hpp"
#include "search/keys.hpp"
#include "send/send.hpp"
#include "send/sendnosign.hpp"
#include "send/sendpoll.hpp"
#include "send/wallet/createfrom.hpp"
#include "send/wallet/rawfrom.hpp"
#include "send/wallet/sendfrom.hpp"
#include "send/wallet/consolidatefrom.hpp"
#include "base/consolidate.hpp"
#include "base/balance.hpp"
#include "search/coins.hpp"
#include "base/seedrandom.hpp"
#include "base/mempool.hpp"
#include "base/cointrack.hpp"
#include "search/txpow.hpp"
#include "search/history.hpp"
#include "send/wallet/createtokenfrom.hpp"
#include "send/wallet/rawtxnfrom.hpp"
#include "send/wallet/constructfrom.hpp"
#include "send/wallet/postfrom.hpp"
#include "send/wallet/signfrom.hpp"

#include "send/sendview.hpp"
#include "send/sendpost.hpp"
#include "send/sendsign.hpp"
#include "txn/txnaddamount.hpp"
#include "txn/txncoinlock.hpp"
#include "txn/txnmine.hpp"
#include "txn/txnminepost.hpp"

#include "txn/txnscript.hpp"
#include "search/tokens.hpp"
#include "base/coincheck.hpp"
#include "base/coinexport.hpp"

#include "txn/txncreate.hpp"
#include "txn/txndelete.hpp"
#include "txn/txnlist.hpp"
#include "txn/txnclear.hpp"
#include "txn/txnstate.hpp"
#include "txn/txnexport.hpp"
#include "txn/txnimport.hpp"
#include "txn/txnview.hpp"

#include "scripts/runscript.hpp"
#include "scripts/scripts.hpp"
#include "scripts/newscript.hpp"
#include "scripts/removescript.hpp"
#include "signatures/sign.hpp"

#include "signatures/verify.hpp"

#include "base/maths.hpp"
#include "base/mmrcreate.hpp"
#include "base/mmrproof.hpp"

#include "base/convert.hpp"
#include "base/random.hpp"
#include "base/checkaddress.hpp"
#include "base/missingcmd.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/json/parser/ParseException.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaExceptions.hpp"
#include "support/NativeThrowable.hpp"
#include "support/CommandResult.hpp"
#include <regex>
namespace minima {
std::vector<std::shared_ptr<Command>> CommandRunner::ALL_COMMANDS={std::make_shared<commands::hashtest>(),std::make_shared<commands::rpc>(),std::make_shared<commands::status>(),std::make_shared<commands::logs>(),std::make_shared<commands::healthcheck>(),std::make_shared<commands::trace>(),std::make_shared<commands::timemilli>(),std::make_shared<commands::network>(),std::make_shared<commands::ping>(),std::make_shared<commands::disconnect>(),std::make_shared<commands::message>(),std::make_shared<commands::p2pstate>(),std::make_shared<commands::quit>(),std::make_shared<commands::rescue>(),std::make_shared<commands::seedrandom>(),std::make_shared<commands::mempool>(),std::make_shared<commands::cointrack>(),std::make_shared<commands::txpow>(),std::make_shared<commands::history>(),std::make_shared<commands::coins>(),std::make_shared<commands::balance>(),std::make_shared<commands::consolidate>(),std::make_shared<commands::consolidatefrom>(),std::make_shared<commands::sendfrom>(),std::make_shared<commands::createtokenfrom>(),std::make_shared<commands::rawfrom>(),std::make_shared<commands::rawtxnfrom>(),std::make_shared<commands::createfrom>(),std::make_shared<commands::constructfrom>(),std::make_shared<commands::postfrom>(),std::make_shared<commands::signfrom>(),std::make_shared<commands::sendpoll>(),std::make_shared<commands::sendnosign>(),std::make_shared<commands::sendview>(),std::make_shared<commands::sendpost>(),std::make_shared<commands::sendsign>(),std::make_shared<commands::send>(),std::make_shared<commands::txnaddamount>(),std::make_shared<commands::txncoinlock>(),std::make_shared<commands::keys>(),std::make_shared<commands::vault>(),std::make_shared<commands::txnsign>(),std::make_shared<commands::txnpost>(),std::make_shared<commands::txnmine>(),std::make_shared<commands::txnminepost>(),std::make_shared<commands::txncheck>(),std::make_shared<commands::txninput>(),std::make_shared<commands::txnbasics>(),std::make_shared<commands::block>(),std::make_shared<commands::printmmr>(),std::make_shared<commands::printtree>(),std::make_shared<commands::automine>(),std::make_shared<commands::txnmmr>(),std::make_shared<commands::txnscript>(),std::make_shared<commands::newaddress>(),std::make_shared<commands::getaddress>(),std::make_shared<commands::coinnotify>(),std::make_shared<commands::slavenode>(),std::make_shared<commands::scripts>(),std::make_shared<commands::newscript>(),std::make_shared<commands::removescript>(),std::make_shared<commands::sign>(),std::make_shared<commands::webhooks>(),std::make_shared<commands::connect>(),std::make_shared<commands::debugflag>(),std::make_shared<commands::txnlock>(),std::make_shared<commands::convert>(),std::make_shared<commands::checkaddress>(),std::make_shared<commands::random>(),std::make_shared<commands::hash>(),std::make_shared<commands::maths>(),std::make_shared<commands::mmrcreate>(),std::make_shared<commands::mmrproof>(),std::make_shared<commands::runscript>(),std::make_shared<commands::verify>(),std::make_shared<commands::txncreate>(),std::make_shared<commands::txndelete>(),std::make_shared<commands::txnlist>(),std::make_shared<commands::txnclear>(),std::make_shared<commands::txnstate>(),std::make_shared<commands::txnexport>(),std::make_shared<commands::txnimport>(),std::make_shared<commands::txnview>(),std::make_shared<commands::txnoutput>(),std::make_shared<commands::tokens>(),std::make_shared<commands::coincheck>(),std::make_shared<commands::coinexport>()};
static std::string trim(const std::string& s){size_t first=0,last=s.size();while(first<last&&uint8_t(s[first])<=32)++first;while(last>first&&uint8_t(s[last-1])<=32)--last;return s.substr(first,last-first);}

json::JSONArray CommandRunner::runMultiCommand(const std::string& command){return runMultiCommand("0x00",command);}
json::JSONObject CommandRunner::runSingleCommand(const std::string& command){auto result=runMultiCommand(command);return result.size()?*std::get<std::shared_ptr<json::JSONObject>>(result.get(0).value):json::JSONObject();}
json::JSONArray CommandRunner::runMultiCommand(const std::string& miniDappID,const std::string& input){
    json::JSONArray finalResult;size_t start=0;
    while(start<input.size()){
        if(input[start]==';'){++start;continue;}auto end=input.find(';',start);if(end==std::string::npos)end=input.size();auto text=trim(input.substr(start,end-start));start=end;
        auto command=getCommand(text);command->setMiniDAPPID(miniDappID);auto params=command->getParams();auto validParams=command->getValidParams();json::JSONObject result;bool valid=true;
        for(const auto& [key,value]:params->entries()){
            auto name=unicode::encodeUTF8(key);
            if(std::find(validParams.begin(),validParams.end(),name)==validParams.end()){
                result.put("command",text);json::JSONValue parameterRef;parameterRef.value=params;result.put("params",parameterRef);result.put("status",false);result.put("pending",false);result.put("error","Invalid parameter : "+name);finalResult.add(result);valid=false;break;
            }
        }
        if(!valid)break;
        auto name=command->getName().value();bool allowed=isCommandAllowed(name);
        if(miniDappID!="0x00"&&!allowed){result.put("command",text);result.put("status",false);result.put("pending",false);result.put("error","You need ADMIN privileges to run this command");finalResult.add(result);break;}
        json::JSONValue reply;
        try{reply=command->runCommand();}
        catch(const CommandException& e){result=command->getJSONReply();result.put("status",false);result.put("error",e.getMessage()?json::JSONValue(*e.getMessage()):json::JSONValue());reply=result;}
        catch(const JavaError&){throw;}
        catch(const std::bad_alloc&){throw;}
        catch(const std::exception& e){MinimaLogger::log(std::make_shared<NativeThrowable>(e));result=command->getJSONReply();result.put("status",false);result.put("error",nativeExceptionText(e));reply=result;}
        finalResult.add(reply);if(!commands::command_result::status(reply))break;
    }
    return finalResult;
}
std::shared_ptr<Command> CommandRunner::getCommandOnly(const std::string& name){for(const auto& command:ALL_COMMANDS){if(!command)throw std::invalid_argument("Null command prototype");if(command->getName()==name)return command->getFunction();}return nullptr;}
std::shared_ptr<Command> CommandRunner::getCommand(const std::string& input){
    auto split=splitStringJSON(false,input);auto name=javaLowerCase(split.at(0));auto command=getCommandOnly(name);if(!command)return std::make_shared<commands::missingcmd>(name,"Command not found");command->setCompleteCommand(input);
    for(size_t i=1;i<split.size();++i){const auto& token=split[i];auto colon=token.find(':');if(colon==std::string::npos)return std::make_shared<commands::missingcmd>(name,"Invalid parameters for "+name+" @ "+token);
        auto key=trim(token.substr(0,colon)),value=trim(token.substr(colon+1));bool isObject=value.starts_with('{')&&value.ends_with('}'),isArray=value.starts_with('[')&&value.ends_with(']');
        if(isObject||(isArray&&name!="txnstate")){
            try{auto parsed=json::parser::JSONParser().parse(value);if(!parsed.isNull()){if(isObject)(void)std::get<std::shared_ptr<json::JSONObject>>(parsed.value);else (void)std::get<std::shared_ptr<json::JSONArray>>(parsed.value);}command->getParams()->put(key,parsed);}
            catch(const json::parser::ParseException& e){return std::make_shared<commands::missingcmd>(name,std::string(isObject?"Invalid JSON parameter for ":"Invalid JSONArray parameter for ")+name+" @ "+token+" org.minima.utils.json.parser.ParseException: "+e.getMessage());}
        }else{
            if(isArray&&name=="txnstate"){command->getParams()->put(key,value);continue;}
            if(value.starts_with('"'))value.erase(0,1);if(value.ends_with('"'))value.pop_back();command->getParams()->put(key,value);
        }
    }
    return command;
}
std::vector<std::string> CommandRunner::splitStringJSON(bool forceNormal,const std::string& input){
    if(!forceNormal&&input.find(":\\")==std::string::npos&&input.find('{')==std::string::npos&&input.find('[')==std::string::npos){auto fast=getRunner().splitterQuotedPattern(input);if(fast)return *fast;}
    std::vector<std::string> tokens;auto text=trim(input);std::string current;int64_t jsonDepth=0;bool quoted=false;
    for(char c:text){
        if(c==' '){if(!quoted&&jsonDepth==0){if(!current.empty())tokens.push_back(trim(current));current.clear();}else current+=c;}
        else if(c=='{'||c=='['){++jsonDepth;current+=c;}else if(c=='}'||c==']'){--jsonDepth;current+=c;}
        else if(c=='"'){if(jsonDepth>0)current+=c;else quoted=!quoted;}else current+=c;
    }
    if(!current.empty())tokens.push_back(trim(current));return tokens;
}
std::optional<std::vector<std::string>> CommandRunner::splitterQuotedPattern(const std::string& input){
    auto trimmed=trim(input);std::string text;for(char c:trimmed){if(c==':')text+=" : ";else text+=c;}
    static const std::regex pattern(R"rx("([^"]*)"|(\S+))rx");std::vector<std::string> tokens;
    for(std::sregex_iterator i(text.begin(),text.end(),pattern),end;i!=end;++i)tokens.push_back((*i)[1].matched?(*i)[1].str():(*i)[2].str());
    std::vector<std::string> result;bool first=true,nameFound=false;std::string pair;
    for(const auto& token:tokens){if(first){first=false;result.push_back(token);}else if(!nameFound){if(trim(token)==":")return std::nullopt;nameFound=true;pair=token;}else if(trim(token)==":")pair+=':';else{nameFound=false;pair+=token;result.push_back(pair);}}
    return result;
}
bool CommandRunner::isCommandAllowed(const std::string& command){
    static const std::vector<std::string> writeCommands={"send","sendpoll","sendsign","multisig","tokencreate","consolidate","cointrack","sign","txnauto","txnsign","mds","backup","removescript","restore","restoresync","vault","archive","mysql","mysqlcoins","rpc","magic","quit","seedrandom","megammrsync","megammr","decryptbackup","reset"};
    return std::find(writeCommands.begin(),writeCommands.end(),trim(command))==writeCommands.end();
}
}
