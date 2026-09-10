#include "connect.hpp"
#include "support/JavaInteger.hpp"
namespace minima::commands {
std::function<void(std::shared_ptr<Message>)> connect::postMessage;
connect::connect():Command("connect","[host:ip:port] - Connect to a network Minima instance"){}
std::optional<std::string> connect::getFullHelp() const {return std::string(
"\nconnect\n"
"\n"
"Connect to a network Minima instance.\n"
"\n"
"Connect to another node to join the main network or to create a private test network.\n"
"\n"
"Set your own host using the -host parameter at start up.\n"
"\n"
"host:\n"
"    The external ip:port of the node to connect to.\n"
"\n"
"Examples:\n"
"\n"
"connect host:94.0.239.117:9001\n");}
std::shared_ptr<Message> connect::createConnectMessage(const std::optional<std::string>& fullHost){
    const auto& text=fullHost.value();auto index=text.find(':');if(index==std::string::npos)return nullptr;
    // Same Java String.trim boundary used by CommandRunner (code units <= 32).
    auto trim=[](const std::string& value){size_t first=0,last=value.size();while(first<last&&uint8_t(value[first])<=32)++first;while(last>first&&uint8_t(value[last-1])<=32)--last;return value.substr(first,last-first);};
    auto host=trim(text.substr(0,index)),ports=trim(text.substr(index+1));int32_t port;
    try{port=javaParseInt(ports);}catch(const JavaNumberFormatException&){return nullptr;}
    auto message=std::make_shared<Message>("NIO_CONNECT");message->addString("host",host).addInteger("port",port);return message;
}
json::JSONValue connect::runCommand(){
    auto result=getJSONReply();const auto& value=getParams()->get("host");if(value.isNull())throw JavaCheckedException("No host specified");
    auto fullHost=value.asString();auto message=createConnectMessage(fullHost);if(!message)throw JavaCheckedException("Must specify host:port");
    postMessage(message);result.put("message","Attempting to connect to "+fullHost);return result;
}
}
