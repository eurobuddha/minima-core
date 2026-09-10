#include "P2PParams.hpp"
namespace minima::p2p {
std::string P2PParams::VERSION="1.1";
std::optional<P2PLogLevel> P2PParams::LOG_LEVEL=P2PLogLevel::NODE_RUNNER_MSG;
int32_t P2PParams::PEERS_LIST_SIZE=50;
int32_t P2PParams::TGT_NUM_LINKS=5;
int32_t P2PParams::TGT_NUM_NONE_P2P_LINKS=100;
int32_t P2PParams::MIN_NUM_CONNECTIONS=2;
int32_t P2PParams::LOOP_DELAY=600000;
int32_t P2PParams::HASH_RATE_UPDATE_DELAY=43200000;
int32_t P2PParams::LOOP_DELAY_VARIABILITY=30000;
int32_t P2PParams::NODE_NOT_ACCEPTING_CHECK_DELAY=3600000;
int32_t P2PParams::SAVE_DATA_DELAY=600000;
int32_t P2PParams::WALK_LINKS_EXPIRE_TIME=10000;
int32_t P2PParams::AUTH_KEY_EXPIRY=300000;
int32_t P2PParams::METRICS_DELAY=600000;
std::shared_ptr<JavaList<SocketAddressPtr>> P2PParams::DEFAULT_NODE_LIST=std::make_shared<JavaList<SocketAddressPtr>>(std::vector<SocketAddressPtr>{},true);
}
