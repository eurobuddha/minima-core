#include "org/minima/system/network/rpc/Authorizer.hpp"
#include "org/minima/system/network/minima/NIOTraffic.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/Base64.hpp"
#include <iostream>
using namespace minima;
static std::optional<std::string> string(const json::JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asString());}
static int64_t number(const json::JSONObject& o,const std::string& k){return std::get<int64_t>(o.get(k).value);}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);auto type=data->get("type").asString();std::string out;
    if(type=="auth"){UserDB db;db.getAllData()->put("rpcusers_allusers",data->get("users"));GeneralParams::RPC_AUTHENTICATE=data->get("authenticate").asBool();GeneralParams::RPC_AUTHSTYLE=data->get("style").asString();GeneralParams::RPC_PASSWORD=string(data->get("password"));out=Authorizer::checkAuchCredentials(string(data->get("header")),db).toJSONString();}
    else if(type=="base64")out=MiniData(decodeBase64(data->get("data").asString())).to0xString();
    else{NIOTraffic traffic;auto ops=std::get<std::shared_ptr<json::JSONArray>>(data->get("ops").value);for(const auto& value:ops->elements()){auto op=std::get<std::shared_ptr<json::JSONObject>>(value.value);auto action=op->get("op").asString();std::string result="OK";try{auto amount=number(*op,"amount");if(action=="reset")traffic.resetAt(amount);else if(action=="read")traffic.addToTotalRead(amount);else if(action=="write")traffic.addToTotalWrite(amount);else if(action=="readkey")traffic.addReadBytes(string(op->get("key")),amount);else if(action=="writekey")traffic.addWriteBytes(string(op->get("key")),amount);else throw std::invalid_argument("Unknown op");}catch(const std::exception&){result="ERROR";}out+=result+":"+std::to_string(traffic.getTotalRead())+":"+std::to_string(traffic.getTotalWrite())+":"+traffic.getBreakdown().toJSONString()+"|";}out+="start:"+std::to_string(traffic.getStartTime());}
    std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
