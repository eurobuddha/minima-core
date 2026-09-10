#include "p2pstate.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
namespace minima::commands {
p2pstate::p2pstate():Command("p2pstate","prints full details of the internal p2p state"){}
std::optional<std::string> p2pstate::getFullHelp() const {return std::string(
"\np2pstate\n"
"\n"
"Prints full details of the internal p2p state.\n"
"\n"
"Includes details of your in and out connections and total peers.\n"
"\n"
"Examples:\n"
"\n"
"p2pstate\n");}
json::JSONValue p2pstate::runCommand(){
    auto result=getJSONReply();auto network=sqlRequired(Main::getInstance()).getNetworkManager();
    auto manager=sqlRequired(network).getP2PManager();auto peer=std::dynamic_pointer_cast<p2p::P2PManager>(manager);
    if(manager&&!peer)throw JavaClassCastException("manager is not a P2PManager");
    result.put("p2p-state",sqlRequired(peer).getStatus(true));return result;
}
}
