#include "P2PDoSwap.hpp"
namespace minima::p2p {
P2PDoSwap::P2PDoSwap(std::shared_ptr<MiniData> secret,SocketAddressPtr target,std::optional<std::string> uid) {
    if(!target)throw std::runtime_error("swapTarget is null");mSecret=std::move(secret);mSwapTarget=std::move(target);mSwappingClientUID=std::move(uid);
}
P2PDoSwap P2PDoSwap::readFromJson(const json::JSONObject& object) {P2PDoSwap result;result.readJson(object);return result;}
void P2PDoSwap::readJson(const json::JSONObject& object) {
    auto contents=std::get<std::shared_ptr<json::JSONObject>>(object.get("do_swap").value);if(!contents)throw std::runtime_error("Null swap contents");
    if(contents->containsKey("secret"))setSecret(std::make_shared<MiniData>(MiniData::fromHex(contents->get("secret").asString())));
    if(contents->containsKey("swap_target_host")&&contents->containsKey("swap_target_port")) {
        const auto& host=contents->get("swap_target_host");std::optional<std::string> name;if(!host.isNull())name=host.asString();setSwapTarget(std::make_shared<InetSocketAddress>(name,InetSocketAddressIO::safeReadInt(*contents,"swap_target_port")));
    }
}
json::JSONObject P2PDoSwap::toJson() const {
    if(!mSecret||!mSwapTarget)throw std::runtime_error("Null swap secret or target");json::JSONObject contents,result;
    contents.put("secret",mSecret->to0xString());contents.put("swap_target_host",mSwapTarget->getHostString());contents.put("swap_target_port",mSwapTarget->getPort());result.put("do_swap",contents);return result;
}
}
