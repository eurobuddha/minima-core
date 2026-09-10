#include "Authorizer.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/Base64.hpp"
#include <iostream>
namespace minima {
std::function<std::shared_ptr<UserDB>()> Authorizer::userDB;
json::JSONObject Authorizer::checkAuchCredentials(const std::optional<std::string>& header) {
    if(!userDB)throw std::logic_error("Authorizer requires MinimaDB binding");auto db=userDB();if(!db)throw std::invalid_argument("Null UserDB");return checkAuchCredentials(header,*db);
}
json::JSONObject Authorizer::checkAuchCredentials(const std::optional<std::string>& header,UserDB& db) {
    json::JSONObject rejected,result;rejected.put("valid",false);result.put("valid",false);
    auto users=db.getRPCUsers();if(!users)throw std::invalid_argument("Null RPC users");(void)users->size();
    if(GeneralParams::RPC_AUTHSTYLE=="basic")try{
        auto position=header.value().find("Basic ");
        if(position!=std::string::npos){
            auto decoded=MiniString(decodeBase64(header->substr(position+6))).toString();
            size_t first=0,last=decoded.size();while(first<last&&uint8_t(decoded[first])<=32)++first;while(last>first&&uint8_t(decoded[last-1])<=32)--last;decoded=decoded.substr(first,last-first);
            auto colon=decoded.find(':');if(colon==std::string::npos)throw std::invalid_argument("Missing credential separator");
            auto username=decoded.substr(0,colon),password=decoded.substr(colon+1);result.put("username",username);
            if(username=="minima"){
                if(!GeneralParams::RPC_AUTHENTICATE||password==GeneralParams::RPC_PASSWORD.load()){result.put("valid",true);result.put("mode","write");return result;}
            }else if(GeneralParams::RPC_AUTHENTICATE){
                users=db.getRPCUsers();if(!users)throw std::invalid_argument("Null RPC users");
                for(const auto& entry:users->elements()){
                    auto user=std::get<std::shared_ptr<json::JSONObject>>(entry.value);if(!user)throw std::invalid_argument("Null RPC user");
                    if(user->getString("username")==username&&user->getString("password")==password){result.put("valid",true);const auto& mode=user->get("mode");result.put("mode",mode.isNull()?json::JSONValue():json::JSONValue(mode.asString()));}
                }
                return result;
            }else std::clog<<"[!] Cannot access rpc user ("<<username<<") as no default password (for user minima) set via -rpcpassword..\n";
        }
    }catch(const std::exception& e){std::clog<<e.what()<<'\n';return rejected;}
    return rejected;
}
}
