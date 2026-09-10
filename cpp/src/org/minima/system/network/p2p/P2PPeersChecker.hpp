#pragma once
#include "P2PFunctions.hpp"
#include "support/JavaRandom48.hpp"
#include "support/SocketAddressSet.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima::p2p {
class P2PPeersChecker : public MessageProcessor {
    std::shared_ptr<SocketAddressSet> unverifiedPeers=std::make_shared<SocketAddressSet>();
    std::shared_ptr<SocketAddressSet> verifiedPeers=std::make_shared<SocketAddressSet>();
    // P2PManager owns this worker; a weak receiver avoids a native ownership cycle.
    std::weak_ptr<MessageStack> p2PManager;
    std::function<std::shared_ptr<TxPoWTreeNode>()> mTip;
    SocketAddressPtr removeRandomItem(const std::shared_ptr<SocketAddressSet>& set);
    void postParent(const std::shared_ptr<Message>& message);
protected:
    P2PPeersChecker(const std::shared_ptr<MessageStack>& manager,std::function<std::shared_ptr<TxPoWTreeNode>()> tip,std::function<void()> releaseLocks);
    void processMessage(std::shared_ptr<Message> message) override;
    virtual int32_t newRandomInt(int32_t bound){return JavaRandom48().nextInt(bound);}
    virtual std::optional<Greeting> ping(const SocketAddressPtr& address);
public:
    inline static std::atomic<int32_t> MAX_VERIFIED_PEERS{250};
    int64_t PEERS_LOOP_TIMER=21600000;
    static constexpr const char *PEERS_INIT="PEERS_INIT",*PEERS_ADDPEERS="PEERS_ADDPEERS",*PEERS_CHECKPEERS="PEERS_CHECKPEERS",*PEERS_LOOP="PEERS_LOOP",*PEERS_FORCEFULLCHECK="PEERS_FORCEFULLCHECK";
    static std::shared_ptr<P2PPeersChecker> create(const std::shared_ptr<MessageStack>& manager,std::function<std::shared_ptr<TxPoWTreeNode>()> tip,std::function<void()> releaseLocks);
    std::shared_ptr<SocketAddressSet> getUnverifiedPeers() const{return unverifiedPeers;}
    std::shared_ptr<SocketAddressSet> getVerifiedPeers() const{return verifiedPeers;}
    bool haveAnyPeers() const{return verifiedPeers->size()>0||unverifiedPeers->size()>0;}
    void checkUnverifiedPeer(const SocketAddressPtr& address);
};
}
