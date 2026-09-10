#include "org/minima/system/commands/CommandRunner.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static JSONArray events;
class Probe:public Command{
public:
    explicit Probe(std::string name):Command(std::move(name),"error fixture"){}
    std::vector<std::string> getValidParams()const override{return {"kind","emit"};}
    std::shared_ptr<Command> getFunction()const override{return std::make_shared<Probe>(getName().value());}
    JSONValue runCommand()override{
        auto kind=getParam("kind","success");events.add(getName().value()+":"+kind);
        if(getBooleanParam("emit",false))MinimaLogger::log("command log");
        if(kind=="command")throw CommandException("command failure");
        if(kind=="runtime")throw JavaTypedException("java.lang.IllegalStateException","runtime failure");
        if(kind=="io")throw wire::IOException("io failure");
        if(kind=="sql")throw SqlException("sql failure");
        if(kind=="sqlruntime")throw SqlRuntimeException("java.lang.IllegalStateException","sql runtime failure");
        if(kind=="format")throw JavaNumberFormatException("format failure");
        if(kind=="arithmetic")throw JavaArithmeticException("arithmetic failure");
        if(kind=="error")throw JavaError("java.lang.LinkageError","error failure");
        if(kind=="oom")throw JavaOutOfMemoryError("memory failure");
        if(kind=="null")return nullptr;
        auto result=getJSONReply();result.put("caller",getMiniDAPPID().value());
        if(kind=="missingstatus"){JSONObject missing;for(const auto& [key,value]:result.entries())if(key!=u"status")missing.put(key,value);return missing;}
        if(kind=="nullstatus")result.put("status",nullptr);
        if(kind=="wrongstatus")result.put("status","true");
        if(kind=="false")result.put("status",false);
        return result;
    }
};
static JSONObject run(const JSONObject& request){
    events=JSONArray();JSONArray logs;int prints=0;int failAt=int(std::get<int64_t>(request.get("failAt").value));auto kind=request.getString("loggerKind");
    MinimaLogger::Runtime logger;
    logger.println=[&](const auto& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at==std::string::npos)return;auto body=text.substr(at+4);if(body.starts_with("     "))return;logs.add(body);if(++prints==failAt){if(kind=="error")throw JavaError("java.lang.LinkageError","logger failure");if(kind=="oom")throw JavaOutOfMemoryError("logger failure");throw JavaTypedException("java.lang.IllegalStateException","logger failure");}};
    MinimaLogger::setRuntime(logger);struct Reset{~Reset(){MinimaLogger::setRuntime({});}}reset;
    JSONValue result;
    try{result=CommandRunner::getRunner().runMultiCommand(request.getString("caller"),request.getString("text"));}
    catch(const std::exception& error){result="escaped:"+nativeExceptionText(error);}
    JSONObject out;out.put("result",result);out.put("events",events);out.put("logs",logs);return out;
}
int main(){
    for(const auto* name:{"fixture","send"})CommandRunner::ALL_COMMANDS.insert(CommandRunner::ALL_COMMANDS.begin(),std::make_shared<Probe>(name));
    std::string line;while(std::getline(std::cin,line))try{auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(*request).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
