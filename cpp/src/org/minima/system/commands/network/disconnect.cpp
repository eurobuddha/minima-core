#include "disconnect.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
namespace minima::commands {
disconnect::disconnect():Command("disconnect","[uid:uid] - Disconnect from a connected or connecting host"){}
std::optional<std::string> disconnect::getFullHelp() const {return std::string(
"\ndisconnect\n"
"\n"
"Disconnect from a connected or connecting host.\n"
"\n"
"Optionally disconnect from all hosts.\n"
"\n"
"uid:\n"
"    Use 'all' to disconnect from all hosts or enter the uid of the host to disconnect from.\n"
"    uid can be found from the 'network' command.\n"
"\n"
"Examples:\n"
"\n"
"disconnect uid:CVNPMLPOCQ0HQ\n"
"\n"
"disconnect uid:all\n");}
json::JSONValue disconnect::runCommand(){
    auto result=getJSONReply();const auto& raw=getParams()->get("uid");
    if(raw.isNull())throw JavaCheckedException("No uid specified");
    if(!raw.isString())throw JavaClassCastException("uid is not a String");auto uid=raw.asString();
    auto nio=sqlRequired(Main::getInstance()).getNIOManager();
    if(uid=="all")sqlRequired(nio).PostMessage(std::make_shared<Message>(NIOManager::NIO_DISCONNECTALL));
    else sqlRequired(nio).disconnect(uid);
    result.put("status",true);result.put("message","Attempting to disconnect from "+uid);return result;
}
}
