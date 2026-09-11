#pragma once
#include "support/P2PEnvironment.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "params/P2PParams.hpp"
namespace minima::p2p {
using ClientInfoPtr=std::shared_ptr<NIOClientInfo>;
using ClientInfoList=std::vector<ClientInfoPtr>;
struct P2PFunctions {
    static void main(const std::vector<std::string>* args=nullptr);
    using Level=P2PLogLevel;
    inline static constexpr const char* P2P_INIT="P2P_INIT",*P2P_SHUTDOWN="P2P_SHUTDOWN",*P2P_CONNECTED="P2P_CONNECTED",*P2P_DISCONNECTED="P2P_DISCONNECTED",*P2P_NOCONNECT="P2P_NOCONNECT",*P2P_MESSAGE="P2P_MESSAGE";
    inline static JavaStringSet mInvalidPeers;
    // Optional platform/log overrides; defaults execute native interfaces/HTTPS.
    inline static std::function<std::shared_ptr<JavaStringSet>()> enumerateInterfaces;
    inline static std::function<void(const std::string&)> openURL,logger;
    static std::shared_ptr<JavaStringSet> getLocalAddresses();
    static void addInvalidPeer(const std::optional<std::string>& host);
    static bool isInvalidPeer(const std::optional<std::string>& host){return mInvalidPeers.contains(host);}
    static void clearInvalidPeers(){mInvalidPeers.clear();}
    static bool isIPv6(const std::optional<std::string>& host);
    static bool isIPLocal(const std::optional<std::string>& host);
    static bool isNetAvailable();
    static void connect(std::optional<std::string> host,int32_t port);
    static bool checkConnect(std::optional<std::string> host,int32_t port);
    static void disconnect(std::optional<std::string> uid);
    static ClientInfoList getAllConnections();
    static ClientInfoList getAllConnectedConnections();
    static ClientInfoPtr getNIOCLientInfo(const std::optional<std::string>& uid);
    static void sendP2PMessage(std::optional<std::string> uid,const std::shared_ptr<json::JSONObject>& message);
    static void sendP2PMessageAll(const std::shared_ptr<json::JSONObject>& message){sendP2PMessage("",message);}
    // Shared native MinimaLogger body adapter until the full logger runtime binds.
    static void log(const std::string& text);
    static void log_node_runner(const std::optional<std::string>& message);
    static void log_info(const std::optional<std::string>& message);
    static void log_debug(const std::optional<std::string>& message);
private:
    static std::shared_ptr<JavaStringSet> mLocalAddresses;
    static std::mutex mLocalLock;
    static std::shared_ptr<NIOManager> manager();
    friend class P2PFunctionsFixture;
};
}
