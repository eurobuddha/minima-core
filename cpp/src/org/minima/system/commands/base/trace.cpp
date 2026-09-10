#include "trace.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
namespace minima::commands {
trace::trace():Command("trace","[enable:true|false] (filter:) (network:) - Show the message stacks of the internal Minima Engine with optional filter string. Only works on terminal."){}
std::optional<std::string> trace::getFullHelp() const {return std::string("\ntrace\n\nShow the message stacks of the internal Minima Engine with optional filter string.\n\nenable:\n    true or false, true to enable or false to disable.\n\nfilter: (optional)\n    A case sensitive string to filter the messages by.\n\nnetwork: (optional)\n    true or false, show low level network messages\n\nExamples:\n\ntrace enable:true\n\ntrace enable:true filter:MAIN\n\ntrace enable:true filter:MINER\n\ntrace enable:true filter:MDS\n\ntrace enable:true filter:MDS network:true\n\ntrace enable:true filter:NOTIFYMANAGER\n\ntrace enable:true filter:TXPOWPROCESSOR\n\ntrace enable:false\n");}
json::JSONValue trace::runCommand(){
    auto result=getJSONReply();const auto& raw=getParams()->get("enable");
    if(raw.isNull())throw JavaCheckedException("Must specify enable");
    if(!raw.isString())throw JavaClassCastException("enable is not a String");
    bool enabled=raw.asString()=="true";auto filter=getParam("filter","");
    MessageProcessor::setTrace(enabled,filter);
    bool network=getBooleanParam("network",false);NIOClient::mTraceON=network;NIOServer::mTraceON=network;
    json::JSONObject response;response.put("enabled",enabled);response.put("filter",filter);response.put("shownetwork",network);
    result.put("response",response);return result;
}
}
