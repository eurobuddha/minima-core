#include "rpc.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/RawTxnCommands.hpp"
#include "support/JavaLowerCase.hpp"
#include <chrono>
#include <thread>
namespace minima::commands {
std::function<void(int64_t)> rpc::sleep=nativeThreadSleep;
rpc::rpc():Command("rpc","(enable:) (ssl:) (password:) - Enable and disable RPC on port "+std::to_string(GeneralParams::RPC_PORT)+" (default is off)"){}
std::optional<std::string> rpc::getFullHelp() const {return "\nrpc\n\nEnable and disable RPC on port " + std::to_string(GeneralParams::RPC_PORT) + " (default is off).\n\nEnsure your RPC port is secured behind a firewall before enabling.\n\nThe Deafult user is minima. That has write mode access. You can add other rpc users with read access.\n\nenable:\n    true or false, true to enable rpc or false to disable.\n\nssl:\n    true or false, true to enable Self signed SSL - you can use stunnel yourself.\n\npassword:\n    the Basic Auth password used in headers - ONLY secure if used with SSL.\n\nusername:\n    the Username of an RPC user.\n\nmode:\n    the read or write mode of an RPC user.\n\naction:\n    adduser - add an RPC user.\n    removeuser - remove an RPC user.\n    listusers - list RPC users.\n\nExamples:\n\nrpc enable:true\n\nrpc enable:true ssl:true password:minimarpcpassword\n\nrpc enable:true password:minima\n\nrpc action:adduser username:rpcuser password:rpcpassword mode:read\n\nrpc action:removeuser username:rpcuser\n\nrpc action:listusers\n\nrpc enable:false\n";}
json::JSONValue rpc::runCommand(){
    auto reply=getJSONReply();auto users=sqlRequired(MinimaDB::getDB()).getUserDB();bool list=false;
    if(existsParam("enable")){
        bool enable=getBooleanParam("enable");
        sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).stopRPC();sleep(1000);
        GeneralParams::RPC_ENABLED=enable;
        if(existsParam("password")){GeneralParams::RPC_PASSWORD=getParam("password");GeneralParams::RPC_AUTHENTICATE=true;}
        else GeneralParams::RPC_AUTHENTICATE=false;
        if(enable)sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).startRPC();
        else sqlRequired(sqlRequired(Main::getInstance()).getNetworkManager()).stopRPC();
    }else if(existsParam("action")){
        auto action=getParam("action");
        if(action=="adduser"){
            if(!GeneralParams::RPC_AUTHENTICATE)throw CommandException("You must set a default RPC password (for user minima) via -rpcpassword to add extra users");
            auto username=getParam("username");if(username=="minima")throw CommandException("Cannot add User minima - this is the default RPC user.");
            auto password=getParam("password");auto mode=javaLowerCase(getParam("mode"));
            if(mode!="read"&&mode!="write")throw CommandException("RPC User mode MUST be either read or write");
            json::JSONObject user;user.put("username",username);user.put("password",password);user.put("mode",mode);
            sqlRequired(sqlRequired(users).getRPCUsers()).add(user);
        }else if(action=="removeuser"){
            auto username=getParam("username");auto current=sqlRequired(users).getRPCUsers();auto replacement=std::make_shared<json::JSONArray>();
            for(const auto& item:sqlRequired(current).elements()){
                const auto& user=raw_detail::object(item);const auto& name=user.get("username");
                if(name.isNull())throw JavaNullPointerException("null RPC username");
                if(raw_detail::stringText(name)!=username)replacement->add(item);
            }
            users->setRPCUsers(replacement);
        }else if(action=="listusers")list=true;
    }
    json::JSONObject details;details.put("enabled",GeneralParams::RPC_ENABLED.load());details.put("port",GeneralParams::RPC_PORT);
    details.put("authenticate",GeneralParams::RPC_AUTHENTICATE.load());details.put("username","minima");details.put("password","***");
    if(!list)details.put("rpcusers",int64_t(sqlRequired(sqlRequired(users).getRPCUsers()).size()));
    else{json::JSONValue array;array.value=sqlRequired(users).getRPCUsers();details.put("rpcusers",array);}
    reply.put("response",details);return reply;
}
}
