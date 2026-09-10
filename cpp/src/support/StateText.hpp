#pragma once
#include "org/minima/utils/json/JSONValue.hpp"
#include "support/JavaFloating.hpp"
namespace minima::commands::send_detail {
inline std::string stateText(const json::JSONValue& value){
    if(value.isString())return value.asString();
    if(auto number=std::get_if<double>(&value.value))return javaFloating(*number);
    return value.toJSONString();
}
}
