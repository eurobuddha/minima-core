#pragma once
#include "P2P2Functions.hpp"
#include "P2P2DB.hpp"
#include "org/minima/system/network/p2p/P2PDB.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include "support/JavaRandom48.hpp"
namespace minima::p2p2 {
struct P2P2ManagerServices {
    std::function<void()> releaseLocks;
    std::function<std::shared_ptr<P2P2DB>()> getP2P2DB;
    std::function<std::shared_ptr<p2p::P2PDB>()> getP2PDB;
};
class P2P2Manager : public MessageProcessor {
    P2P2ManagerServices mServices;
protected:
    explicit P2P2Manager(P2P2ManagerServices services);
    void initialize();
    void processMessage(std::shared_ptr<Message> message) override;
public:
    inline static std::string P2P2_INIT="P2P2_INIT",P2P2_SHUTDOWN="P2P2_SHUTDOWN",P2P2_FASTLOOP="P2P2_FAST_LOOP",P2P2_SLOWLOOP="P2P2_SLOW_LOOP";
    std::atomic<int64_t> P2P2_LOOP_TIMER{300000},P2P2_LOOP_TIMER_SLOW{21600000};
    std::atomic<int32_t> NUMBER_DESIRED_CONNECTIONS{3};
    static std::shared_ptr<P2P2Manager> create(P2P2ManagerServices services);
    void shutdown();
    std::optional<std::string> getRandomPeerFromList();
    void checkWhichConnect();
    void convertOldP2P();
};
}
