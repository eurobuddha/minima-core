#include "network.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::commands {
network::network():Command("network","(action:list|reset|recalculateip) - Show network status or reset traffic counter"){}
std::optional<std::string> network::getFullHelp() const {return std::string(
"\nnetwork\n\nShow network status or reset traffic counter.\n\naction: (optional)\n"
"    list : List the direct peers you are connected to. The default.\n"
"    reset : Restart the traffic counter from 0.\n"
"    recalculateip : Reset your IP - when you move to a different WiFi.\n"
"\nExamples:\n\nnetwork\n\nnetwork action:list\n\nnetwork action:reset\n");}
json::JSONValue network::runCommand(){
    auto result=getJSONReply();auto action=getParam("action","list");
    if(action=="list"){
        auto uid=getParam("uid","");
        auto clients=sqlRequired(sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).getNIOManager()).getAllConnectionInfo();
        json::JSONArray connections;
        for(const auto& info:clients){if(uid.empty())connections.add(info.toJSON());else if(info.getUID()==uid){connections.add(info.toJSON());break;}}
        json::JSONObject response;response.put("connections",connections);
        auto manager=sqlRequired(Main::getInstance()).getNetworkManager();if(manager)response.put("details",manager->getStatus(true));
        result.put("response",response);
    }else if(action=="reset"){
        sqlRequired(sqlRequired(sqlRequired(Main::getInstance()).getNIOManager()).getTrafficListener()).reset();
        result.put("response","Traffic counter restarted..");
    }else if(action=="recalculateip"){
        sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).calculateHostIP();
        json::JSONObject ip;ip.put("ip",GeneralParams::MINIMA_HOST);result.put("response",ip);
    }else if(action=="restart"){
        sqlRequired(Main::getInstance()).PostMessage(Main::MAIN_NETRESTART);result.put("response","Restarting..");
    }else if(action=="loggingon"||action=="loggingoff"){
        bool enabled=action=="loggingon";
        sqlRequired(sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).getNIOManager()).setFullLogging(enabled,"");
        sqlRequired(sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).getP2PManager()).setFullLogging(enabled,"");
        result.put("response",enabled?"Full Network logging ON":"Full Network logging OFF");
    }else throw CommandException("Invalid action");
    return result;
}
}
