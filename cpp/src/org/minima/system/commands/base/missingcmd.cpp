#include "missingcmd.hpp"
namespace minima::commands {
json::JSONValue missingcmd::runCommand(){auto result=getJSONReply();result.put("command",mInput?json::JSONValue(*mInput):json::JSONValue());result.put("status",false);result.put("error",mError?json::JSONValue(*mError):json::JSONValue());return result;}
std::shared_ptr<Command> missingcmd::getFunction() const {return std::make_shared<missingcmd>("","");}
}
