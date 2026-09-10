#pragma once
#include "InetSocketAddressIO.hpp"
#include "org/minima/objects/base/MiniData.hpp"
namespace minima::p2p {
class P2PDoSwap {
    std::shared_ptr<MiniData> mSecret=std::make_shared<MiniData>(MiniData::getRandomData(8));
    SocketAddressPtr mSwapTarget;
    std::optional<std::string> mSwappingClientUID;
public:
    P2PDoSwap()=default;
    P2PDoSwap(std::shared_ptr<MiniData> secret,SocketAddressPtr target,std::optional<std::string> uid);
    static P2PDoSwap readFromJson(const json::JSONObject& object);
    void readJson(const json::JSONObject& object);
    json::JSONObject toJson() const;
    const auto& getSecret() const {return mSecret;}
    void setSecret(std::shared_ptr<MiniData> secret) {mSecret=std::move(secret);}
    const auto& getSwapTarget() const {return mSwapTarget;}
    void setSwapTarget(SocketAddressPtr target) {mSwapTarget=std::move(target);}
    const auto& getSwappingClientUID() const {return mSwappingClientUID;}
    void setSwappingClientUID(std::optional<std::string> uid) {mSwappingClientUID=std::move(uid);}
};
}
