#include "P2PTestParams.hpp"
namespace minima::p2p {
P2PTestParams& P2PTestParams::values(){static P2PTestParams settings;return settings;}
void P2PTestParams::setTestParams(){
    const auto& settings=values();
    P2PParams::DEFAULT_NODE_LIST=settings.DEFAULT_NODE_LIST;
    P2PParams::LOOP_DELAY=settings.LOOP_DELAY;
    P2PParams::LOOP_DELAY_VARIABILITY=settings.LOOP_DELAY_VARIABILITY;
    P2PParams::NODE_NOT_ACCEPTING_CHECK_DELAY=settings.NODE_NOT_ACCEPTING_CHECK_DELAY;
    P2PParams::WALK_LINKS_EXPIRE_TIME=settings.WALK_LINKS_EXPIRE_TIME;
    P2PParams::AUTH_KEY_EXPIRY=settings.AUTH_KEY_EXPIRY;
    P2PParams::TGT_NUM_LINKS=settings.TGT_NUM_LINKS;
    P2PParams::TGT_NUM_NONE_P2P_LINKS=settings.TGT_NUM_NONE_P2P_LINKS;
    P2PParams::MIN_NUM_CONNECTIONS=settings.MIN_NUM_CONNECTIONS;
    P2PParams::METRICS_DELAY=settings.METRICS_DELAY;
    P2PParams::SAVE_DATA_DELAY=settings.SAVE_DATA_DELAY;
    P2PParams::HASH_RATE_UPDATE_DELAY=settings.HASH_RATE_UPDATE_DELAY;
    P2PParams::LOG_LEVEL=settings.LOG_LEVEL;
}
}
