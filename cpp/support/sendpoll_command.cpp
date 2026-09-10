#include "org/minima/system/commands/send/sendpoll.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
class Poll:public SendPollManager {
public:
    JSONArray timers;
    Poll():SendPollManager([](const auto& text){if(!text)throw JavaNullPointerException("null command");return CommandRunner::getRunner().runSingleCommand(*text);},[]{}){}
    void poll(){processMessage(std::make_shared<Message>(SENDPOLL_FUNCTION));}
protected:
    void schedulePoll() override {JSONObject item;item.put("type",SENDPOLL_FUNCTION);item.put("delay",SENDPOLL_TIMER.load());timers.add(item);}
};
static const JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static const JSONArray& array(const JSONValue& value){return *std::get<std::shared_ptr<JSONArray>>(value.value);}
static std::optional<std::string> string(const JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asString());}
static JSONArray snapshot(const std::vector<SendPollMessage>& messages){JSONArray result;for(const auto& message:messages)result.add(message.toJSON());return result;}
static JSONObject run(const JSONObject& input){
    auto manager=std::make_shared<Poll>();bool missing=false,nullManager=false;
    commands::sendpoll::getManager=[&]()->std::shared_ptr<SendPollManager>{if(missing)throw JavaNullPointerException("null Main receiver");return nullManager?nullptr:manager;};
    struct Reset{~Reset(){commands::sendpoll::getManager=[]()->std::shared_ptr<SendPollManager>{throw JavaNullPointerException("null Main receiver");};MinimaLogger::setRuntime({});}}reset;
    MinimaLogger::Runtime logger;logger.println=[](const auto&){};MinimaLogger::setRuntime(logger);
    std::vector<SendPollMessage> held;JSONArray outputs;
    for(const auto& entry:array(input.get("ops")).elements()){
        const auto& op=object(entry);auto kind=op.getString("op");JSONObject out;
        try{
            JSONValue value;
            if(kind=="main"){missing=op.containsKey("missing")&&op.get("missing").asBool();nullManager=op.containsKey("nullManager")&&op.get("nullManager").asBool();}
            else if(kind=="hold")held=manager->listCommands();
            else if(kind=="stop")manager->stopMessageProcessor();
            else if(kind=="poll")manager->poll();
            else if(kind=="run")value=CommandRunner::getRunner().runMultiCommand(op.containsKey("caller")?op.getString("caller"):"0x00",op.getString("text"));
            else if(kind=="meta"){
                commands::sendpoll command;JSONObject result;result.put("help",command.getFullHelp().value());JSONArray valid;for(const auto& param:command.getValidParams())valid.add(param);result.put("valid",valid);result.put("reply",command.getJSONReply());result.put("complete",command.getCompleteCommand().value());auto fresh=command.getFunction();result.put("fresh",fresh.get()!=&command&&fresh->getParams()!=command.getParams());value=result;
            }else{
                commands::sendpoll command;if(op.containsKey("text"))command.setCompleteCommand(string(op.get("text")));
                auto params=op.containsKey("params")?object(op.get("params")):JSONObject();
                if(op.containsKey("removeIndex")){params.put("action","remove");params.put("uid",held.at(size_t(std::get<int64_t>(op.get("removeIndex").value))).getUID().value());}
                *command.getParams()=params;value=command.runCommand();
            }
            out.put("value",value);
        }catch(const CommandException& error){out.put("exception",std::string("org.minima.system.commands.CommandException: ")+error.what());}
        catch(const std::exception& error){out.put("exception",nativeExceptionText(error));}
        out.put("queue",snapshot(manager->listCommands()));out.put("held",snapshot(held));out.put("timers",JSONArray(manager->timers));outputs.add(out);
    }
    JSONObject result;result.put("ops",outputs);return result;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
