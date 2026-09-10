#pragma once
#include "org/minima/system/commands/CommandRunner.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::commands::command_result {
// Java wrappers select element zero, unlike runSingleCommand's empty fallback.
inline json::JSONObject first(const std::string& command){
    auto replies=CommandRunner::getRunner().runMultiCommand(command);
    if(replies.size()==0)throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");
    const auto& value=replies.get(0);
    auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value);
    if(!object)throw JavaClassCastException("Cannot cast command response to JSONObject");
    if(!*object)throw JavaNullPointerException("null command response");
    return **object;
}
[[noreturn]] inline void reject(const json::JSONObject& result){
    const auto& value=result.get("error");
    if(value.isNull())throw CommandException(nullptr);
    if(!value.isString())throw JavaClassCastException("Cannot cast command error to String");
    throw CommandException(value.asString());
}
inline bool booleanValue(const json::JSONValue& value){
    if(value.isNull())throw JavaNullPointerException("null command status");
    auto boolean=std::get_if<bool>(&value.value);
    if(!boolean)throw JavaClassCastException("Cannot cast command status to Boolean");
    return *boolean;
}
inline bool status(const json::JSONObject& result){return booleanValue(result.get("status"));}
inline bool status(const json::JSONValue& result){
    if(result.isNull())throw JavaNullPointerException("null command response");
    auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&result.value);
    if(!object)throw JavaClassCastException("Cannot cast command response to JSONObject");
    if(!*object)throw JavaNullPointerException("null command response");
    return status(**object);
}

}
