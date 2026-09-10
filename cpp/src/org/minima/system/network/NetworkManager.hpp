#pragma once
#include "minima/NIOManager.hpp"
#include "p2p/P2PManager.hpp"
#include "p2p2/P2P2Manager.hpp"
#include "rpc/HTTPServer.hpp"
#include "org/minima/database/userprefs/UserDB.hpp"
namespace minima {
struct NetworkManagerServices {
    NIOManagerServices nio;
    p2p::P2PManagerServices p2p;
    p2p2::P2P2ManagerServices p2p2;
    std::function<std::shared_ptr<UserDB>()> getUserDB;
    std::function<std::vector<p2p::HostInterface>()> enumerateInterfaces=p2p::localHostInterfaces;
    std::function<int64_t()> currentTimeMillis=TimerMessage::currentTimeMillis;
};
class NetworkManager : public std::enable_shared_from_this<NetworkManager> {
    NetworkManagerServices mServices;
    std::shared_ptr<NIOManager> mNIOManager;
    std::shared_ptr<MessageProcessor> mP2PManager,mP2P2Manager;
    mutable std::mutex mOwnerLock;
    std::shared_ptr<HTTPServer> mRPCServer;
    mutable std::mutex mRPCLock;
    void initialize();
protected:
    explicit NetworkManager(NetworkManagerServices services):mServices(std::move(services)){}
public:
    std::atomic<bool> mShuttingDown{false};
    static std::shared_ptr<NetworkManager> create(NetworkManagerServices services);
    ~NetworkManager();
    void calculateHostIP();
    json::JSONObject getStatus(bool all=false);
    void startRPC();
    void stopRPC();
    void shutdownNetwork();
    bool isShutDownComplete() const;
    void hardShutDown();
    std::shared_ptr<MessageProcessor> getP2PManager() const{std::lock_guard lock(mOwnerLock);return mP2PManager;}
    std::shared_ptr<MessageProcessor> getP2P2Manager() const{std::lock_guard lock(mOwnerLock);return mP2P2Manager;}
    std::shared_ptr<NIOManager> getNIOManager() const{std::lock_guard lock(mOwnerLock);return mNIOManager;}
    // Native ownership/ephemeral-port observers, supplementing the Java methods.
    void join();
    int32_t getBoundRPCPort() const;
    friend class NetworkManagerFixture;
};
}
