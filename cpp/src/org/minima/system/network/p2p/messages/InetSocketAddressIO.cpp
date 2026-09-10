#include "InetSocketAddressIO.hpp"
#include "support/JavaInteger.hpp"
namespace minima::p2p {
json::JSONArray InetSocketAddressIO::addressesListToJSON(const SocketAddressList& peers) {
    if(!peers)throw std::runtime_error("Null peer list");json::JSONArray result;
    for(const auto& address:*peers){if(!address)throw std::runtime_error("Null peer address");json::JSONObject value;value.put("host",address->getHostAddress());value.put("port",address->getPort());result.add(value);}return result;
}
json::JSONArray InetSocketAddressIO::addressesListToJSONArray(const SocketAddressList& peers) {
    if(!peers)throw std::runtime_error("Null peer list");json::JSONArray result;
    for(const auto& address:*peers){if(!address)throw std::runtime_error("Null peer address");result.add(address->getHostAddress()+":"+std::to_string(address->getPort()));}return result;
}
SocketAddressList InetSocketAddressIO::addressesJSONToList(const std::shared_ptr<json::JSONArray>& peers) {
    if(!peers)throw std::runtime_error("Null peer array");auto result=std::make_shared<std::vector<SocketAddressPtr>>();
    for(const auto& value:peers->elements()) {auto object=std::get<std::shared_ptr<json::JSONObject>>(value.value);if(!object)throw std::runtime_error("Null peer object");const auto& host=object->get("host");std::optional<std::string> name;if(!host.isNull())name=host.asString();result->push_back(std::make_shared<InetSocketAddress>(name,safeReadInt(*object,"port")));}return result;
}
SocketAddressList InetSocketAddressIO::addressesJSONArrayToList(const std::shared_ptr<json::JSONArray>& peers) {
    if(!peers)throw JavaNullPointerException("null peer array");auto result=std::make_shared<std::vector<SocketAddressPtr>>();
    for(const auto& value:peers->elements()){if(value.isNull())throw JavaNullPointerException("null host-port string");if(!value.isString())throw JavaClassCastException("peer is not a String");auto text=value.asString();std::vector<std::string> parts;size_t start=0;while(true){auto end=text.find(':',start);if(end==std::string::npos){parts.push_back(text.substr(start));break;}parts.push_back(text.substr(start,end-start));start=end+1;}if(!text.empty())while(!parts.empty()&&parts.back().empty())parts.pop_back();if(parts.size()<2)throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(parts.empty()?0:1)+" out of bounds for length "+std::to_string(parts.size()));auto host=parts[0];int port=javaParseInt(parts[1]);result->push_back(std::make_shared<InetSocketAddress>(host,port));}return result;
}
int32_t InetSocketAddressIO::safeReadInt(const json::JSONObject& object,const std::string& key) {
    const auto& value=object.get(key);auto integer=std::get_if<int64_t>(&value.value);if(!integer)return 0;
    if(*integer<INT32_MIN||*integer>INT32_MAX)throw std::overflow_error("integer overflow");return int32_t(*integer);
}
}
