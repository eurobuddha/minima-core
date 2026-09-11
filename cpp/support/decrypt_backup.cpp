#include "org/minima/system/commands/backup/decryptbackup.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
JSONObject run(const JSONObject& request){
    JSONObject out;
    try{
        GeneralParams::BASE_FILE_FOLDER=request.getString("folder");commands::decryptbackup command;auto op=request.getString("op","");
        if(op=="meta"){
            out.put("name",*command.getName());out.put("help",*command.getHelp());out.put("fullHelp",*command.getFullHelp());
            JSONArray params;for(const auto& name:command.getValidParams())params.add(name);out.put("params",params);
            command.getParams()->put("file","fixture");out.put("freshParams",*command.getFunction()->getParams());
        }else if(op=="runner")out.put("result",CommandRunner::getRunner().runMultiCommand(request.getString("caller"),request.getString("command")));
        else{
            auto params=std::get<std::shared_ptr<JSONObject>>(request.get("params").value);
            for(const auto& [key,value]:params->entries())command.getParams()->put(key,value);
            out.put("result",command.runCommand());
        }
    }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException")+(error.getMessage()?": "+*error.getMessage():""));}
    catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}return out;
}
int main(){std::string line;while(std::getline(std::cin,line)){auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<std::endl;}}
