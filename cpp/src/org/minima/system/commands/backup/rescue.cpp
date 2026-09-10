#include "rescue.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::commands {
rescue::rescue():Command("rescue","(host:) - Perform a megammrsync with the -rescuenode parameter if specified (or 'host' param)..."){}
json::JSONValue rescue::runCommand(){
    auto result=getJSONReply();std::string host;
    if(existsParam("host"))host=getParam("host");
    else if(!GeneralParams::RESCUE_MEGAMMR_NODE.empty())host=GeneralParams::RESCUE_MEGAMMR_NODE;
    if(host.empty())throw CommandException("No valid rescue / megammr node specified..");
    auto message=std::make_shared<Message>(Main::MAIN_DO_RESCUE);message->addString("host",host);
    sqlRequired(Main::getInstance()).PostMessage(message);
    json::JSONObject response;response.put("host",host);response.put("message","Rescue attempt started..");
    result.put("response",response);return result;
}
}
