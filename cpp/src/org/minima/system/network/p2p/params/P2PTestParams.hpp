#pragma once
#include "P2PParams.hpp"
namespace minima::p2p {
class P2PTestParams {
    P2PTestParams()=default;
public:
    // One mutable settings object models Java static fields. First access also
    // initializes the address list, without resolving it before test-mode use.
    static P2PTestParams& values();
    P2PTestParams(const P2PTestParams&)=delete;
    P2PTestParams& operator=(const P2PTestParams&)=delete;
    std::optional<P2PLogLevel> LOG_LEVEL=P2PLogLevel::DEBUG;
    int32_t TGT_NUM_LINKS=5;
    int32_t TGT_NUM_NONE_P2P_LINKS=2;
    int32_t MIN_NUM_CONNECTIONS=2;
    int32_t LOOP_DELAY=5000;
    int32_t HASH_RATE_UPDATE_DELAY=60000;
    int32_t LOOP_DELAY_VARIABILITY=3000;
    int32_t NODE_NOT_ACCEPTING_CHECK_DELAY=60000;
    int32_t SAVE_DATA_DELAY=60000;
    int32_t WALK_LINKS_EXPIRE_TIME=5000;
    int32_t AUTH_KEY_EXPIRY=300000;
    int32_t METRICS_DELAY=30000;
    std::optional<std::string> METRICS_URL="http://metrics:5000/network";
    std::shared_ptr<JavaList<SocketAddressPtr>> DEFAULT_NODE_LIST=std::make_shared<JavaList<SocketAddressPtr>>(
        std::vector<SocketAddressPtr>{std::make_shared<InetSocketAddress>("minima_one",9001)},true);
    static void setTestParams();
};
}
