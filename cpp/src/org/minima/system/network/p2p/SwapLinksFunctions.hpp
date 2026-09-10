#pragma once
#include "UtilFuncs.hpp"
#include "messages/P2PGreeting.hpp"
#include "messages/P2PWalkLinks.hpp"
namespace minima::p2p {
struct SwapLinksFunctions {
    using Messages=std::vector<std::shared_ptr<Message>>;
    // Required Main-presence receiver, distinct from a null NIOManager.
    inline static std::function<bool()> mainPresent;
    static json::JSONObject wrapP2PMsg(const std::shared_ptr<json::JSONObject>& data);
    static Messages onConnected(const std::shared_ptr<P2PState>& state,bool incoming,const std::shared_ptr<NIOClient>& info);
    static Messages onConnectedLoadBalanceRequest(const std::shared_ptr<P2PState>& state,const ClientInfoList& clients);
    static void onDisconnected(const std::shared_ptr<P2PState>& state,const std::shared_ptr<Message>& message);
    static bool processGreeting(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PGreeting>& greeting,const ClientInfoPtr& client,bool noconnect);
    static json::JSONObject processRequestIPMsg(const std::shared_ptr<json::JSONObject>& message,const std::optional<std::string>& host);
    static void processResponseIPMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<json::JSONObject>& message);
    static Messages joinScaleOutLinks(const std::shared_ptr<P2PState>& state,int32_t target,const ClientInfoList& clients);
    static Messages requestInLinks(const std::shared_ptr<P2PState>& state,int32_t target,const ClientInfoList& clients);
};
}
