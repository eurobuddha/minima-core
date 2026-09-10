#pragma once
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
namespace minima::p2p2 {
struct P2P2Functions {
    enum class Level {NODE_RUNNER_MSG,INFO,DEBUG};
    inline static constexpr const char *P2P_INIT="P2P_INIT",*P2P_SHUTDOWN="P2P_SHUTDOWN",*P2P_CONNECTED="P2P_CONNECTED",*P2P_DISCONNECTED="P2P_DISCONNECTED",*P2P_NOCONNECT="P2P_NOCONNECT",*P2P_MESSAGE="P2P_MESSAGE";
    inline static JavaStringSet mInvalidPeers;
    inline static std::function<std::shared_ptr<JavaStringSet>()> enumerateInterfaces;
    static std::shared_ptr<JavaStringSet> getLocalAddresses();
    static void addInvalidPeer(const std::optional<std::string>& host);
    static bool isInvalidPeer(const std::optional<std::string>& host){return mInvalidPeers.contains(host);}
    static void clearInvalidPeers(){mInvalidPeers.clear();}
    static bool isIPv6(const std::optional<std::string>& host){return p2p::P2PFunctions::isIPv6(host);}
    static bool isIPLocal(const std::optional<std::string>& host){return p2p::P2PFunctions::isIPLocal(host);}
    static bool isNetAvailable(){return p2p::P2PFunctions::isNetAvailable();}
    static void connect(std::optional<std::string> host,int32_t port){p2p::P2PFunctions::connect(std::move(host),port);}
    static bool checkConnect(std::optional<std::string> host,int32_t port);
    static void disconnect(std::optional<std::string> uid){p2p::P2PFunctions::disconnect(std::move(uid));}
    static p2p::ClientInfoList getAllConnections(){return p2p::P2PFunctions::getAllConnections();}
    static p2p::ClientInfoList getAllConnectedConnections(){return p2p::P2PFunctions::getAllConnectedConnections();}
    static p2p::ClientInfoPtr getNIOCLientInfo(const std::optional<std::string>& uid){return p2p::P2PFunctions::getNIOCLientInfo(uid);}
    static void main(const std::vector<std::string>& args);
private:
    inline static std::shared_ptr<JavaStringSet> mLocalAddresses;
    inline static std::mutex mLocalLock;
    friend class P2P2FunctionsFixture;
};
}
