#include "incentivecash.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
namespace minima::commands {
incentivecash::incentivecash():Command("incentivecash","(uid:) - Show your rewards or specify your UserID for the Incentive Cash program"){}
std::optional<std::string> incentivecash::getFullHelp() const {return std::string(
"\nincentivecash\n"
"\n"
"Returns your Incentive Program Rewards balance and full breakdown of daily, invite and community Rewards.\n"
"\n"
"Set your Incentive ID with the 'uid' parameter to start receiving daily Rewards.\n"
"\n"
"uid: (optional)\n"
"    Your Incentive Program ID, can be found by logging into the incentive.minima.global website.\n"
"\n"
"Examples:\n"
"\n"
"incentivecash\n"
"\n"
"incentivecash uid:00d11b34-7b47-45f3-775c-a37cbe4c9ff3\n");}
std::string incentivecash::requestPUT(const std::string& url){return RPCClient::sendPUT(url);}
json::JSONValue incentivecash::runCommand(){
    auto result=getJSONReply();
    if(existsParam("uid")){
        auto uid=getParam("uid");sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).setIncentiveCashUserID(uid);
        sqlRequired(MinimaDB::getDB()).saveUserDB();
    }
    auto user=sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getIncentiveCashUserID();
    json::JSONObject response;response.put("uid",user);
    if(!user.empty()){
        auto reply=requestPUT("https://incentivecash.minima.global/api/ping/"+user+"?version="+GlobalParams::MINIMA_VERSION);
        auto parsed=json::parser::JSONParser().parse(reply);
        if(!parsed.isNull()&&!std::holds_alternative<std::shared_ptr<json::JSONObject>>(parsed.value))throw JavaClassCastException("incentive response is not JSONObject");
        response.put("details",parsed);
    }
    result.put("response",response);return result;
}
}
