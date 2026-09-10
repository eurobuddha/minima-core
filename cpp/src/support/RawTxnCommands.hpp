#pragma once
#include "support/CommandResult.hpp"
#include "support/SqlConnection.hpp"
#include "support/StateText.hpp"
#include "support/JavaInteger.hpp"
#include "org/minima/objects/StateVariable.hpp"
namespace minima::commands::raw_detail {
inline const json::JSONObject& object(const json::JSONValue& value){
    if(value.isNull())throw JavaNullPointerException("null raw coin object");
    auto result=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value);
    if(!result)throw JavaClassCastException("Cannot cast raw coin to JSONObject");
    return sqlRequired(*result);
}
// These source casts precede string concatenation, which renders null as text.
inline std::string stringText(const json::JSONValue& value){
    if(value.isNull())return "null";
    if(!value.isString())throw JavaClassCastException("Cannot cast raw field to String");
    return value.asString();
}
template<class CommandType> void outputs(CommandType& command,const std::string& id,const std::shared_ptr<json::JSONArray>& outputs){
    for(const auto& item:sqlRequired(outputs).elements()){
        const auto& output=object(item);auto address=stringText(output.get("address"));auto amount=stringText(output.get("amount"));
        auto token=output.containsKey("tokenid")?stringText(output.get("tokenid")):"0x00";
        auto store=output.containsKey("storestate")?command_result::booleanValue(output.get("storestate")):true;
        command.runCommand("txnoutput id:"+id+" address:"+address+" amount:"+amount+" tokenid:"+token+" storestate:"+(store?"true":"false"));
    }
}
template<class CommandType> void state(CommandType& command,const std::string& id,const std::shared_ptr<json::JSONObject>& state){
    if(!state)return;
    for(const auto& [key,value]:state->entries()){
        auto port=javaParseInt(unicode::encodeUTF8(key));auto text=send_detail::stateText(value);StateVariable stateValue(port,text);
        command.runCommand("txnstate id:"+id+" port:"+std::to_string(stateValue.getPort())+" value:\""+stateValue.getData().toString()+"\"");
    }
}
}
