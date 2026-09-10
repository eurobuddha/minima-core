#pragma once
#include "support/InetSocketAddress.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima::p2p {
struct InetSocketAddressIO {
    static json::JSONArray addressesListToJSON(const SocketAddressList& peers);
    static json::JSONArray addressesListToJSONArray(const SocketAddressList& peers);
    static SocketAddressList addressesJSONToList(const std::shared_ptr<json::JSONArray>& peers);
    static SocketAddressList addressesJSONArrayToList(const std::shared_ptr<json::JSONArray>& peers);
    static int32_t safeReadInt(const json::JSONObject& object,const std::string& key);
};
}
