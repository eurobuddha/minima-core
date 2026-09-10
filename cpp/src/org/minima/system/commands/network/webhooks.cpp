#include "webhooks.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands {
std::function<std::shared_ptr<NotifyManager>()> webhooks::runtimeLookup=[]()->std::shared_ptr<NotifyManager>{throw JavaNullPointerException("Main notification manager is not bound");};
webhooks::webhooks():Command("webhooks","(action:list|add|remove|clear) (hook:url) - Add a web hook that is called with Minima events as they happen"){}
std::optional<std::string> webhooks::getFullHelp() const {return std::string(
"\nwebhooks\n"
"\n"
"Add a web hook that is called with Minima events as they happen.\n"
"\n"
"POST requests, so the URL must be a POST endpoint.\n"
"\n"
"action: (optional)\n"
"    list : List your existing webhooks. The default.\n"
"    add : Add a new webhook. \n"
"    remove : Remove an existing webhook.\n"
"    clear : Clear the existing webhooks.\n"
"\n"
"hook: (optional)\n"
"    A URL, must be a POST endpoint.\n"
"\n"
"filter: (optional)\n"
"    Filters which events get posted.\n"
"\n"
"Examples:\n"
"\n"
"webhooks action:list\n"
"\n"
"webhooks action:add hook:http://127.0.0.1/myapi.php\n"
"\n"
"webhooks action:remove hook:http://127.0.0.1/myapi.php\n"
"\n"
"webhooks action:add hook:http://127.0.0.1/myapi.php filter:MINING\n"
"\n"
"webhooks action:clear\n");}
json::JSONValue webhooks::runCommand(){
    auto result=getJSONReply();auto action=getParamUTF16("action",u"list");auto notify=runtimeLookup();
    auto response=std::make_shared<json::JSONObject>();json::JSONValue ref;ref.value=response;result.put("response",ref);
    auto required=[](const auto& owner)->NotifyManager&{if(!owner)throw JavaNullPointerException("null notification manager");return *owner;};
    if(action==u"add"){auto filter=getParamUTF16("filter",u"");auto hook=getParamUTF16("hook");required(notify).addHookUTF16(filter+u"#"+hook);}
    else if(action==u"remove"){auto hook=getParamUTF16("hook");required(notify).removeHookUTF16(hook);}
    else if(action==u"clear")required(notify).clearHooks();
    else if(action==u"errorlogs"){bool enable=getParamUTF16("enable")==u"true";required(runtimeLookup()).WEBHOOKS_ERROR_LOGS=enable;}
    response->put("webhooks",*MiniUtil::convertUTF16ArrayList(required(notify).getAllWebHooks()));return result;
}
}
