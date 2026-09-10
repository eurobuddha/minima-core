#include "ping.hpp"
#include "org/minima/system/Main.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaStringTrim.hpp"
namespace minima::commands {
ping::ping():Command("ping","[host:] - Ping a host and get back Minima Node info"){}
std::optional<std::string> ping::getFullHelp() const {return std::string("\nping\n\nPing a host and get back Minima Node info.\n\nExamples:\n\nping host:\n");}
json::JSONValue ping::runCommand(){
    auto result=getJSONReply();auto host=getParam("host");auto index=host.find(':');
    if(index==std::string::npos)return nullptr;
    auto ip=javaStringTrim(host.substr(0,index)),ports=javaStringTrim(host.substr(index+1));int32_t port=0;
    try{port=javaParseInt(ports);}catch(const JavaNumberFormatException&){throw CommandException("Invalid port : "+ports);}
    // Java evaluates this receiver expression even though sendPingMessage is static.
    (void)sqlRequired(Main::getInstance()).getNIOManager();
    auto greeting=NIOManager::sendPingMessage(ip,port,false);json::JSONObject response;
    response.put("host",ip);response.put("port",port);response.put("valid",bool(greeting));
    if(greeting){response.put("version",greeting->getVersion().toString());const auto& extra=greeting->getExtraDataOptional();response.put("extradata",extra?json::JSONValue(*extra):json::JSONValue());}
    result.put("response",response);return result;
}
}
