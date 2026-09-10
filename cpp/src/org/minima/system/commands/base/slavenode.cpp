#include "slavenode.hpp"
#include "org/minima/database/MinimaDB.hpp"
namespace minima::commands {
slavenode::slavenode():Command("slavenode","Run in slavenode mode. Requires a master node to connect to."){}
std::optional<std::string> slavenode::getFullHelp() const {return "\nslavenode\n\nConnect to a master node and receive txblock messages.\n\nExamples:\n\nslavenode enable:true host:87.34.45.56:9001\n";}
std::vector<std::string> slavenode::getValidParams() const {return {"host","enable"};}
json::JSONValue slavenode::runCommand(){
    auto result=getJSONReply();auto user=sqlRequired(MinimaDB::getDB()).getUserDB();json::JSONObject response;
    if(existsParam("enable")){
        response.put("message","RESTART REQUIRED");auto enabled=getBooleanParam("enable");
        if(enabled){auto host=getParam("host");sqlRequired(user).setSlaveNode(enabled,host);}
        else sqlRequired(user).setSlaveNode(enabled,"");
    }
    sqlRequired(MinimaDB::getDB()).saveUserDB();
    auto enabled=sqlRequired(user).isSlaveNode();auto host=user->getSlaveNodeHost();
    response.put("enabled",enabled);response.put("master",host);result.put("response",response);return result;
}
std::shared_ptr<Command> slavenode::getFunction() const {return std::make_shared<slavenode>();}
}
