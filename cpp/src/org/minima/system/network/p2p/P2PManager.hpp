#pragma once
#include "P2PPeersChecker.hpp"
#include "P2PDB.hpp"
#include "SwapLinksFunctions.hpp"
#include "WalkLinksFuncs.hpp"
namespace minima::p2p {
// Required external MinimaDB receivers; full Main/MinimaDB bind these at startup.
struct P2PManagerServices {
    std::function<void()> releaseLocks,saveP2PDB;
    std::function<std::shared_ptr<P2PDB>()> getP2PDB;
    std::function<std::shared_ptr<TxPoWTreeNode>()> getTip;
    std::function<int64_t()> currentTimeMillis=TimerMessage::currentTimeMillis;
};
class P2PManager : public MessageProcessor {
public:
    using Messages=SwapLinksFunctions::Messages;
    static constexpr const char *P2P_RESET="P2P_RESET",*P2P_RANDOM_CONNECT="P2P_RANDOM_CONNECT",
        *P2P_LOOP="P2P_LOOP",*P2P_ASSESS_CONNECTIVITY="P2P_ASSESS_CONNECTIVITY",
        *P2P_SEND_MSG="P2P_SEND_MSG",*P2P_SEND_MSG_TO_ALL="P2P_SEND_MSG_TO_ALL",
        *P2P_SEND_CONNECT="P2P_SEND_CONNECT",*P2P_SEND_DISCONNECT="P2P_SEND_DISCONNECT",
        *P2P_ADD_PEER="P2P_ADD_PEER",*P2P_REMOVE_PEER="P2P_REMOVE_PEER",
        *P2P_SAVE_DATA="P2P_SAVE_DATA",*P2P_HEALTH_CHECK="P2P_HEALTH_CHECK",*ADDRESS_LITERAL="address";
    std::atomic<int32_t> LAST_SAVED_PEERS_AMOUNT{0},INITIAL_PEERS_LIST_NUM{0};
    std::atomic<int64_t> P2P_HEALTH_CHECK_TIMER{600000};
private:
    inline static JavaRandom48 rand;
    P2PManagerServices mServices;
    const std::shared_ptr<P2PState> state=std::make_shared<P2PState>();
    std::vector<Message::Key> EXCLUDE_FROM_CLEAR;
    std::shared_ptr<P2PPeersChecker> mPeersChecker;
    bool firstgo=false;
    int64_t mLastNotifyNoPeers=0,NOTIFY_NOPEERS_TIMER=180000;
    void doDiscoveryPing();
    Messages processLoop(const std::shared_ptr<P2PState>& state);
    void sendMessages(const Messages& messages);
    friend class P2PManagerFixture;
protected:
    explicit P2PManager(P2PManagerServices services);
    // Factory phase, called only after shared ownership and before manager start.
    void initialize();
    virtual std::shared_ptr<P2PPeersChecker> createPeersChecker();
    virtual void pauseDiscovery();
    void processMessage(std::shared_ptr<Message> message) override;
    Messages init(const std::shared_ptr<P2PState>& state);
    Messages processJsonMessages(const std::shared_ptr<Message>& message,const std::shared_ptr<P2PState>& state);
    static Messages assessConnectivity(const std::shared_ptr<P2PState>& state);
    static Messages processWalkLinksMsg(const std::shared_ptr<json::JSONObject>& message,const ClientInfoPtr& client,const std::shared_ptr<P2PState>& state);
    static Messages processReturningMessage(const std::shared_ptr<P2PWalkLinks>& walk,const std::shared_ptr<P2PState>& state);
    static std::shared_ptr<Message> processOutgoingWalkMessage(const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& client,const std::shared_ptr<P2PState>& state);
public:
    static std::shared_ptr<P2PManager> create(P2PManagerServices services);
    ~P2PManager() override;
    std::shared_ptr<P2PPeersChecker> getPeersChecker() const{return mPeersChecker;}
    SocketAddressList getPeersCopy() const{return state->getKnownPeersCopy();}
    std::string getP2PAddress() const;
    float getClients() const;
    static Messages connect(const std::shared_ptr<Message>& message,const std::shared_ptr<P2PState>& state);
    json::JSONObject getStatus(bool fullDetails);
    void updateP2PPeersList();
    void shutdown();
    bool haveAnyPeers();
};
}
