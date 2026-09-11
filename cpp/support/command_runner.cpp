#include "org/minima/system/commands/CommandRunner.hpp"
#include <iostream>
using namespace minima;
class Mock:public Command{public:explicit Mock(std::string name):Command(std::move(name),"fixture"){}std::vector<std::string> getValidParams()const override{return {"data","value","fail","state","text","path","amount"};}json::JSONValue runCommand()override{if(getBooleanParam("fail",false))throw CommandException("fixture failed");auto result=getJSONReply();result.put("caller",getMiniDAPPID()?json::JSONValue(*getMiniDAPPID()):json::JSONValue());return result;}std::shared_ptr<Command> getFunction()const override{return std::make_shared<Mock>(getName().value());}};
int main(){std::cout<<std::unitbuf;for(const auto* name:{"fixture","send","txnstate"})CommandRunner::ALL_COMMANDS().insert(CommandRunner::ALL_COMMANDS().begin(),std::make_shared<Mock>(name));std::string line;while(std::getline(std::cin,line))try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);auto op=data->get("op").asString(),text=data->get("text").asString();auto runner=CommandRunner::getRunner();json::JSONValue result;
    if(op=="split"){auto split=runner.splitterQuotedPattern(text);if(split){json::JSONArray a;for(const auto& t:*split)a.add(t);result=a;}}
    else if(op=="direct"||op=="meta"){
        auto command=runner.getCommandOnly(text);if(command){
            if(op=="direct"){for(const auto& [key,value]:std::get<std::shared_ptr<json::JSONObject>>(data->get("params").value)->entries())command->getParams()->put(key,value);result=command->runCommand();}
            else{json::JSONObject meta;meta.put("help",command->getHelp().value());meta.put("fullhelp",command->getFullHelp().value());json::JSONArray valid;for(const auto& p:command->getValidParams())valid.add(p);meta.put("valid",valid);meta.put("fresh",command->getFunction()->getJSONReply());result=meta;}
        }
    }
    else if(op=="allowed")result=runner.isCommandAllowed(text);
    else if(op=="run")result=runner.runMultiCommand(data->get("caller").asString(),text);
    else if(op=="single")result=runner.runSingleCommand(text);
    else{auto command=op=="only"?runner.getCommandOnly(text):runner.getCommand(text);if(command){json::JSONObject object;object.put("name",command->getName()?json::JSONValue(*command->getName()):json::JSONValue());object.put("params",*command->getParams());object.put("complete",command->getCompleteCommand()?json::JSONValue(*command->getCompleteCommand()):json::JSONValue());if(command->getName()=="missingcmd")object.put("reply",command->runCommand());result=object;}}
    std::cout<<result.toJSONString()<<'\n';
}catch(const std::exception&){std::cout<<"\"ERROR\"\n";}}
