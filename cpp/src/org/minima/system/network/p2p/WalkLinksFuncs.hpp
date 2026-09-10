#pragma once
#include "UtilFuncs.hpp"
#include "messages/P2PWalkLinks.hpp"
#include "messages/P2PDoSwap.hpp"
namespace minima::p2p {
class WalkLinksFuncs {
    inline static JavaRandom48 random;
    WalkLinksFuncs()=delete;
    friend class P2PFunctionsFixture;
public:
    using Messages=std::vector<std::shared_ptr<Message>>;
    static SocketAddressList removeIPsInBFromA(const PeerMapPtr& a,const SocketAddressList& b);
    static std::shared_ptr<Message> onOutLinkWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& from,int32_t target,const ClientInfoList& clients);
    static std::shared_ptr<Message> createNextHopMsg(const SocketAddressPtr& destination,const std::shared_ptr<P2PWalkLinks>& walk,const std::shared_ptr<P2PState>& state);
    static std::shared_ptr<Message> generateDoSwapMessageFromWalk(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,int32_t target);
    static SocketAddressPtr getAddressFromUID(const std::shared_ptr<P2PState>& state,const std::optional<std::string>& uid);
    static std::shared_ptr<Message> onInLinkWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& from,const ClientInfoList& clients);
    static std::shared_ptr<Message> onWalkLinkResponseMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk);
    static Messages onReturnedWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,int32_t target);
    static Messages onReturnedLoadBalanceWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk);
    static Messages genLoadBalanceDoSwaps(const std::shared_ptr<P2PState>& state,const SocketAddressPtr& address,int32_t maximum);
};
}
