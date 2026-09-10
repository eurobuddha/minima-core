#pragma once
#include "support/InetSocketAddress.hpp"
#include "support/JavaList.hpp"
#include "support/P2PLogLevel.hpp"
namespace minima::p2p {
struct P2PParams {
    static std::string VERSION;
    static std::optional<P2PLogLevel> LOG_LEVEL;
    static int32_t PEERS_LIST_SIZE;
    static int32_t TGT_NUM_LINKS;
    static int32_t TGT_NUM_NONE_P2P_LINKS;
    static int32_t MIN_NUM_CONNECTIONS;
    static int32_t LOOP_DELAY;
    static int32_t HASH_RATE_UPDATE_DELAY;
    static int32_t LOOP_DELAY_VARIABILITY;
    static int32_t NODE_NOT_ACCEPTING_CHECK_DELAY;
    static int32_t SAVE_DATA_DELAY;
    static int32_t WALK_LINKS_EXPIRE_TIME;
    static int32_t AUTH_KEY_EXPIRY;
    static int32_t METRICS_DELAY;
    static std::shared_ptr<JavaList<SocketAddressPtr>> DEFAULT_NODE_LIST;
};
}
