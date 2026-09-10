#pragma once
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
#include <iostream>
using namespace minima;using namespace minima::p2p;using namespace minima::json;
static auto object(const JSONValue& value) {auto ptr=std::get<std::shared_ptr<JSONObject>>(value.value);if(!ptr)throw std::runtime_error("Null object");return ptr;}
static auto array(const JSONValue& value) {if(value.isNull())return std::shared_ptr<JSONArray>{};return std::get<std::shared_ptr<JSONArray>>(value.value);}
static auto string(const JSONValue& value) {return value.isNull()?std::optional<std::string>{}:std::optional(value.asString());}
static JSONValue nullable(const std::optional<std::string>& value) {return value?JSONValue(*value):JSONValue{};}
static SocketAddressPtr address(const JSONValue& value) {if(value.isNull())return nullptr;auto s=object(value);int port=int(std::get<int64_t>(s->get("port").value));return s->containsKey("unresolved")&&s->get("unresolved").asBool()?InetSocketAddress::createUnresolved(string(s->get("host")),port):std::make_shared<InetSocketAddress>(string(s->get("host")),port);}
static JSONValue snapshot(const SocketAddressPtr& address) {if(!address)return {};JSONObject result;result.put("host",address->getHostString());result.put("address",address->isUnresolved()?JSONValue{}:JSONValue(address->getHostAddress()));result.put("port",address->getPort());result.put("text",address->toString());result.put("hash",address->hashCode());return result;}
static SocketAddressList list(const JSONValue& value) {if(value.isNull())return nullptr;auto result=std::make_shared<std::vector<SocketAddressPtr>>();for(const auto& item:array(value)->elements())result->push_back(address(item));return result;}
static JSONValue snapshots(const SocketAddressList& values) {if(!values)return {};JSONArray result;for(const auto& value:*values)result.add(snapshot(value));return result;}
