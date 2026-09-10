#include "debugflag.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::commands {
debugflag::debugflag():Command("debugflag","(activate:true|false) (var:) - Set DEBUG flag for testing code.."){}
std::vector<std::string> debugflag::getValidParams() const {return {"activate","var"};}
json::JSONValue debugflag::runCommand() {
    auto result=getJSONReply();auto active=getParam("activate","false");
    GeneralParams::DEBUGFLAG=active=="true";
    if(existsParam("var")) GeneralParams::DEBUGVAR=getParam("var");
    json::JSONObject response;response.put("debug",GeneralParams::DEBUGFLAG);response.put("var",GeneralParams::DEBUGVAR);result.put("response",response);return result;
}
std::shared_ptr<Command> debugflag::getFunction() const {return std::make_shared<debugflag>();}
}
