#include "message.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
namespace minima::commands {
message::message():Command("message","[data:message] (uid:uid) - Send a message over the network to one of your direct peers"){}
std::optional<std::string> message::getFullHelp() const {return std::string(
"\nmessage\n"
"\n"
"Send a message to one or all of your direct peers.\n"
"\n"
"data:\n"
"    The message as a string.\n"
"\n"
"uid: (optional)\n"
"    Leave blank to send a message to all peers or enter the uid of the peer to send the message to.\n"
"    uid can be found from the 'network' command.\n"
"\n"
"Examples:\n"
"\n"
"message data:\"hello\" uid:CVNPMLPOCQ0HQ\n");}
json::JSONValue message::runCommand(){
    auto result=getJSONReply();const auto& raw=getParams()->get("data");
    if(raw.isNull())throw JavaCheckedException("No data specified");
    if(!raw.isString())throw JavaClassCastException("data is not a String");auto data=raw.asString();
    const auto& recipient=getParams()->get("uid");std::string uid;
    if(recipient.isNull())result.put("message","Message sent to all");
    else {if(!recipient.isString())throw JavaClassCastException("uid is not a String");uid=recipient.asString();result.put("message","Message sent to "+uid);}
    MiniString payload(data);
    // Java evaluates the receiver expression even for this static NIO method.
    auto nio=sqlRequired(Main::getInstance()).getNIOManager();(void)nio;
    NIOManager::sendNetworkMessage(uid,NIOMessage::MSG_GENMESSAGE,payload);return result;
}
}
