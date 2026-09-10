#pragma once
#include "org/minima/system/network/NetworkManager.hpp"
namespace minima::support {
struct MainNetworkFixture {
    virtual ~MainNetworkFixture()=default;
    virtual std::shared_ptr<NetworkManager> network()=0;
    virtual json::JSONObject snapshot()=0;
    static std::shared_ptr<MainNetworkFixture> create(const json::JSONObject& config,const std::function<void(std::shared_ptr<NetworkManager>)>& publish);
};
}
