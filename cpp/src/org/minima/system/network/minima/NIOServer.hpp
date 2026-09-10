#pragma once
#include "NIOClient.hpp"
#include "org/minima/utils/messages/MessageStack.hpp"
namespace minima {
class SocketWakeup;
class NIOServer {
    friend class NIOManagerFixture;
    using Wakeup=SocketWakeup;
    struct Registration {
        std::shared_ptr<NIOClient> client;
        std::shared_ptr<PosixNIOSocket> socket;
        std::shared_ptr<NativeNIOSelectionKey> key;
    };
    std::function<void(std::shared_ptr<Message>)> mPostMessage;
    int32_t mPort;
    std::atomic<int32_t> mBoundPort{0};
    std::shared_ptr<Wakeup> mSelector;
    mutable std::mutex mSelectorLock;
    std::shared_ptr<Wakeup> selector() const {std::lock_guard lock(mSelectorLock);return mSelector;}
    std::atomic<bool> mShutDown{false},mIsRunning{false};
    JavaConcurrentMap<std::shared_ptr<NIOClient>> mClients;
    std::mutex mRegisterLock,mDisconnectLock;
    std::vector<std::shared_ptr<PosixNIOSocket>> mRegisterChannels;
    std::vector<std::optional<std::string>> mDisconnectChannels;
    std::vector<std::shared_ptr<Registration>> mRegistrations;
    void wakeup();
    void post(std::shared_ptr<Message> message);
    void addChannel(bool incoming,const std::shared_ptr<PosixNIOSocket>& socket);
    void removeRegistration(const std::shared_ptr<NIOClient>& client);
public:
    static std::atomic<bool> mTraceON;
    NIOServer(int32_t port,std::shared_ptr<MessageStack> manager):NIOServer(port,[manager=std::move(manager)](auto message){if(!manager)throw std::runtime_error("Null NIO manager");manager->PostMessage(std::move(message));}){}
    // Runtime receiver may retain the owner weakly; a running thread retains
    // the manager until it exits, avoiding a permanent manager/server cycle.
    NIOServer(int32_t port,std::function<void(std::shared_ptr<Message>)> receiver):mPostMessage(std::move(receiver)),mPort(port){}
    ~NIOServer();
    void shutdown();
    int32_t getNetClientSize() const {return int32_t(mClients.size());}
    std::vector<std::shared_ptr<NIOClient>> getAllNIOClients() const {return mClients.elements();}
    void regsiterNewSocket(std::shared_ptr<PosixNIOSocket> channel);
    void disconnect(std::optional<std::string> uid);
    std::shared_ptr<NIOClient> getClient(const std::optional<std::string>& uid) const {return mClients.get(uid.value()).value_or(nullptr);}
    void sendMessage(const std::optional<std::string>& uid,const std::shared_ptr<MiniData>& data);
    void sendMessageAll(const std::shared_ptr<MiniData>& data);
    bool isRunning() const {return mIsRunning;}
    void run();
    // Native observation for a kernel-assigned port; mPort retains Java's input.
    int32_t getBoundPort() const {return mBoundPort;}
};
}
