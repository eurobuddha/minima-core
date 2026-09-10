#include "org/minima/system/network/webhooks/NotifyManager.hpp"
#include "SqlRecordingFixture.hpp"
#include "org/minima/system/commands/network/webhooks.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "support/JavaIOExceptionText.hpp"
#include <iostream>
using namespace minima;
using namespace minima::sql_fixture;
namespace {
class Manual:public NotifyManager {
public:
    explicit Manual(NotifyManagerServices s):NotifyManager(std::move(s)){}
    using NotifyManager::processMessage;using MessageStack::getNextMessage;
    std::deque<std::shared_ptr<Message>> queue(){std::lock_guard lock(mLock);return mMessages;}
};
class Listener:public MessageListener {
public:std::function<void(std::shared_ptr<Message>)> call;
    void processMessage(std::shared_ptr<Message> msg)override{call(std::move(msg));}
};
std::string errorText(const std::exception& e){if(dynamic_cast<const CommandException*>(&e))return std::string("org.minima.system.commands.CommandException: ")+e.what();if(auto io=dynamic_cast<const wire::IOException*>(&e))return javaIOExceptionText(*io);return javaExceptionText(e);}
auto wide(const JSONValue& v)->std::optional<std::u16string>{return v.isNull()?std::nullopt:std::optional(v.asUTF16());}
JSONValue run(const JSONObject& input){
    std::array<std::shared_ptr<UserDB>,2> users{std::make_shared<UserDB>(),std::make_shared<UserDB>()};
    users[0]->setWebHooksUTF16(MiniUtil::convertUTF16JSONArray(std::get<std::shared_ptr<JSONArray>>(input.get("hooks").value)));
    std::vector<std::shared_ptr<JSONObject>> notifications;
    for(const auto& v:array(input.get("notifications")).elements())notifications.push_back(v.isNull()?nullptr:std::get<std::shared_ptr<JSONObject>>(v.value));
    auto listener=std::make_shared<Listener>();std::shared_ptr<Manual> manager;const JSONObject* command=nullptr;int dbIndex=0,sends=0;JSONArray trace,logs;std::vector<std::shared_ptr<Message>> held;
    auto record=[&](std::initializer_list<JSONValue> values){JSONArray row;for(auto v:values)row.add(v);trace.add(row);};
    auto flag=[&](const std::string& key){return command&&!command->get(key).isNull()&&command->get(key).asBool();};
    auto mutate=[&](const std::string& where){auto action=command->getString(where,"");if(action=="clear")manager->clearHooks();else if(action=="add")manager->addHookUTF16(wide(command->get("hook")));else if(action=="remove")manager->removeHookUTF16(wide(command->get("hook")));else if(action=="stop")manager->shutDown();};
    NotifyManagerServices services;services.releaseLocks=[]{};
    services.getUserDB=[&]{record({"user",dbIndex});if(flag("failDB"))throw std::runtime_error("fixture DB failure");return flag("nullDB")?std::shared_ptr<UserDB>():users[dbIndex];};
    services.getListener=[&]()->std::shared_ptr<MessageListener>{return flag("listener")?listener:nullptr;};
    services.sendPOST=[&](const auto& url,const auto& body){record({"post",url,body});++sends;if(sends==1)mutate("postAction");if(flag("postFailure"))throw wire::IOException("fixture POST failure");return std::string();};
    services.log=[&](const auto& text,bool notify){JSONArray row;row.add(text);row.add(notify);logs.add(row);record({"log",text,notify});if(notify&&flag("logFailure"))throw std::runtime_error("fixture log failure");};
    manager=std::make_shared<Manual>(services);
    commands::webhooks::runtimeLookup=[&]()->std::shared_ptr<NotifyManager>{record({"manager"});return flag("nullManager")?nullptr:manager;};
    listener->call=[&](auto msg){record({"listener",msg->toString()});auto n=msg->getObject("notify").template as<std::shared_ptr<JSONObject>>();if(command->containsKey("listenerEvent"))n->put("event",command->get("listenerEvent"));mutate("listenerAction");if(command->containsKey("replace")){auto next=notifications.at(size_t(integer(*command,"replace")));msg->addObject("notify",MessageObject(next));}if(flag("listenerFailure"))throw std::runtime_error("fixture listener failure");};
    JSONArray output;
    for(const auto& value:array(input.get("ops")).elements()){
        command=&object(value);trace=JSONArray();logs=JSONArray();sends=0;manager->WEBHOOKS_ERROR_LOGS=flag("logs");auto prior=users[dbIndex]->getAllData()->get("webhooks");auto priorDB=dbIndex;JSONObject row;
        try{JSONValue result;auto cmd=command->getString("cmd");auto index=integer(*command,"event");auto notification=index<0?nullptr:notifications.at(size_t(index));
            if(cmd=="command")result=CommandRunner::getRunner().runMultiCommand(command->getString("caller","0x00"),command->getString("text"));
            else if(cmd=="direct"){commands::webhooks action;for(const auto& [key,v]:object(command->get("params")).entries())action.getParams()->put(key,v);result=action.runCommand();}
            else if(cmd=="meta"){commands::webhooks prototype;auto copy=prototype.getFunction();JSONObject meta;meta.put("name",copy->getName().value());meta.put("help",copy->getHelp().value());meta.put("full",copy->getFullHelp().value());JSONArray valid;for(auto name:copy->getValidParams())valid.add(name);meta.put("valid",valid);result=meta;}
            else if(cmd=="add")manager->addHookUTF16(wide(command->get("hook")));
            else if(cmd=="remove")manager->removeHookUTF16(wide(command->get("hook")));
            else if(cmd=="clear")manager->clearHooks();
            else if(cmd=="get")result=*MiniUtil::convertUTF16ArrayList(manager->getAllWebHooks());
            else if(cmd=="queue")manager->PostEvent(notification);
            else if(cmd=="next"){auto msg=manager->getNextMessage();held.push_back(msg);manager->processMessage(msg);}
            else if(cmd=="process"){auto msg=std::make_shared<Message>(NotifyManager::NOTIFY_POST);msg->addObject("notify",MessageObject(notification));held.push_back(msg);manager->processMessage(msg);}
            else if(cmd=="unknown")manager->processMessage(std::make_shared<Message>("UNKNOWN"));
            else if(cmd=="wrong"){auto msg=std::make_shared<Message>(NotifyManager::NOTIFY_POST);msg->addObject("notify",int64_t(1));manager->processMessage(msg);}
            else if(cmd=="mutate")notification->put("event",command->get("value"));
            else if(cmd=="stop")manager->shutDown();
            else if(cmd=="rebind")dbIndex=int(integer(*command,"db"));
            else if(cmd=="setDB")users[dbIndex]->setWebHooksUTF16(MiniUtil::convertUTF16JSONArray(std::get<std::shared_ptr<JSONArray>>(command->get("hooks").value)));
            else if(cmd=="copy"){auto copy=manager->getAllWebHooks();copy.clear();result=*MiniUtil::convertUTF16ArrayList(copy);}
            else throw std::invalid_argument(cmd);
            row.put("value",result);
        }catch(const std::exception& e){row.put("exception",errorText(e));}
        row.put("errorlogs",manager->WEBHOOKS_ERROR_LOGS.load());row.put("trace",trace);row.put("logs",logs);row.put("running",manager->isRunning());
        try{row.put("hooks",*MiniUtil::convertUTF16ArrayList(manager->getAllWebHooks()));}catch(const std::exception& e){row.put("hooksError",errorText(e));}
        JSONArray stored;for(auto db:users)stored.add(*db->getAllData());row.put("db",stored);row.put("dbIndex",dbIndex);
        const auto& current=users[priorDB]->getAllData()->get("webhooks");auto identity=[](const auto& v){return v.isNull()?std::shared_ptr<JSONArray>():std::get<std::shared_ptr<JSONArray>>(v.value);};row.put("replaced",identity(prior)!=identity(current));
        JSONArray pending,retained,state;for(auto msg:manager->queue())pending.add(msg?JSONValue(msg->toString()):JSONValue());for(auto msg:held)retained.add(msg?JSONValue(msg->toString()):JSONValue());for(auto n:notifications)state.add(n?JSONValue(*n):JSONValue());row.put("pending",pending);row.put("retained",retained);row.put("notifications",state);output.add(row);
    }
    manager->shutDown();commands::webhooks::runtimeLookup=[]()->std::shared_ptr<NotifyManager>{throw JavaNullPointerException("unbound fixture");};JSONObject result;result.put("ops",output);return result;
}
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
