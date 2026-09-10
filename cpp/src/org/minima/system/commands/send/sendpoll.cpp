#include "sendpoll.hpp"
#include "support/SqlConnection.hpp"
namespace minima::commands {
sendpoll::sendpoll():Command("sendpoll","(action:add|list|remove) (uid:) - Send function that is added to a list and polls until complete"){}
std::optional<std::string> sendpoll::getFullHelp() const {return std::string(
"\nsendpoll\n"
"\n"
"Send function that adds 'send' commands to a list and polls every 30 seconds until the return status is 'true'.\n"
"\n"
"Accepts the same parameters as the 'send' function.\n"
"\n"
"action: (optional)\n"
"    list : list all the 'send' commands in the polling list.\n"
"    remove : remove a 'send' command from the polling list.\n"
"\n"
"uid: (optional)\n"
"    The uid of a 'send' command you wish to remove from the polling list. Use with 'action:remove'.\n"
"\n"
"Examples:\n"
"\n"
"sendpoll address:0xFF.. amount:10\n"
"\n"
"sendpoll address:0xFF.. amount:10 tokenid:0xFED5.. burn:0.1\n"
"\n"
"sendpoll action:list\n"
"\n"
"sendpoll action:remove uid:0x..\n");}
std::vector<std::string> sendpoll::getValidParams() const {return {"action","uid","address","amount","multi","tokenid","state","burn","coinage","split","debug","dryrun","mine","password","storestate","fromaddress","signkey"};}
std::shared_ptr<Command> sendpoll::getFunction() const {return std::make_shared<sendpoll>();}
json::JSONValue sendpoll::runCommand(){
    auto result=getJSONReply();auto action=getParam("action","add");
    json::JSONObject response;auto manager=getManager();
    if(action=="add"){
        const auto& complete=getCompleteCommand();
        if(!complete)throw JavaNullPointerException("null complete command");
        auto text=*complete;auto position=text.find("sendpoll");
        if(position!=std::string::npos)text.replace(position,8,"send");
        if(text=="send")throw CommandException("Must be a valid send command..");
        sqlRequired(manager).addSendCommand(text);response.put("command",text);
    }else if(action=="list"){
        json::JSONArray list;for(const auto& item:sqlRequired(manager).listCommands())list.add(item.toJSON());
        response.put("commands",list);response.put("total",int32_t(list.size()));
    }else if(action=="remove"){
        auto uid=getParam("uid");sqlRequired(manager).removeCommand(uid);response.put("removed",uid);
    }
    result.put("response",response);return result;
}
}
