#pragma once
#include "NIOServer.hpp"
#include "NIOClientInfo.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/objects/Greeting.hpp"
#include "support/FixedTaskExecutor.hpp"
#include "support/NIOManagerServices.hpp"
namespace minima {
class NIOManager : public MessageProcessor {
    NIOManagerServices mServices;
    std::shared_ptr<NIOServer> mNIOServer;
    std::shared_ptr<NIOTraffic> mTrafficListener=std::make_shared<NIOTraffic>();
    JavaConcurrentMap<std::shared_ptr<NIOClient>> mConnectingClients;
    FixedTaskExecutor mThreadPool{4};
    std::thread mServerThread;
    std::mutex mServerJoinLock;
    int64_t SYNC_MAX_TIME;
    static void sendSerialized(const std::optional<std::string>& uid,const MiniByte& type,const std::function<wire::Bytes()>& serializer);
    static std::shared_ptr<NIOManager> mainManager();
    std::shared_ptr<NIOManager> self(){return std::static_pointer_cast<NIOManager>(shared_from_this());}
    void startServer();
    void joinServer();
protected:
    explicit NIOManager(NIOManagerServices services);
    void processMessage(std::shared_ptr<Message> message) override;
    virtual void connectAttempt(const std::shared_ptr<NIOClient>& client);
    virtual void schedule(const std::shared_ptr<TimerMessage>& message){PostTimerMessage(message);}
public:
    static std::function<std::shared_ptr<NIOManager>()> runtimeLookup;
    static constexpr const char *NIO_SERVERSTARTED="NIO_SERVERSTARTED",*NIO_SHUTDOWN="NIO_SHUTDOWN",*NIO_CONNECT="NIO_CONNECT",*NIO_CONNECTATTEMPT="NIO_CONNECTATTEMPT",*NIO_NEWCONNECTION="NIO_NEWCONNECT",*NIO_DISCONNECT="NIO_DISCONNECT",*NIO_DISCONNECTED="NIO_DISCONNECTED",*NIO_DISCONNECTALL="NIO_DISCONNECTALL",*NIO_RECONNECT="NIO_RECONNECT",*NIO_INCOMINGMSG="NIO_NEWMSG",*NIO_TXPOWREQ="NIO_REQTXPOW",*NIO_SYNCTXBLOCK="NIO_SYNCTXBLOCK",*NIO_CHECKLASTMSG="NIO_CHECKLASTMSG",*NIO_HEALTHCHECK="NIO_HEALTHCHECK";
    inline static int64_t MAX_ARCHIVE_WRITE=1024*1024*50;
    int32_t RECONNECT_ATTEMPTS=3;
    int64_t LASTREAD_CHECKER=120000,MAX_LASTREAD_CHECKER=600000,NIO_HEALTHCHECK_TIMER=1200000,MAX_TIP_TIME_GAP=7200000;
    static constexpr int64_t RECONNECT_TIMER=30000;
    static std::shared_ptr<NIOManager> create(NIOManagerServices services);
    ~NIOManager() override;
    std::shared_ptr<NIOServer> getNIOServer() const{return mNIOServer;}
    int32_t getNumberOfConnectedClients() const{return mNIOServer->getNetClientSize();}
    int32_t getNumberOfConnnectingClients() const{return int32_t(mConnectingClients.size());}
    std::vector<NIOClientInfo> getAllConnectionInfo() const;
    std::vector<std::shared_ptr<NIOClient>> getAllValidConnectedClients() const;
    std::vector<std::shared_ptr<NIOClient>> getAllValidOutGoingConnectedClients() const;
    json::JSONObject getAllConnectedDetails() const;
    std::shared_ptr<NIOClient> checkConnected(const std::optional<std::string>& host,bool onlyConnected) const;
    std::shared_ptr<NIOClient> getMaximaUID(const std::optional<std::string>& key) const;
    std::shared_ptr<NIOClient> getNIOClient(const std::optional<std::string>& host) const;
    std::shared_ptr<NIOClient> getNIOClientFromUID(const std::optional<std::string>& uid) const;
    void hardShutDown();
    std::shared_ptr<NIOTraffic> getTrafficListener() const{return mTrafficListener;}
    void disconnect(std::optional<std::string> uid,bool removeP2P=false);
    // The node runtime binds NIOClient::runtimeLookup to the active manager.
    std::shared_ptr<NIORuntime> clientRuntime();
    static MiniData createNIOMessage(const MiniByte& type,const std::function<wire::Bytes()>& serializer);
    template<class T> requires requires(const T& object){object.serialise();}
    static MiniData createNIOMessage(const MiniByte& type,const T& object){return createNIOMessage(type,std::function<wire::Bytes()>([&]{return object.serialise();}));}
    template<class T> static MiniData createNIOMessage(const MiniByte& type,const std::shared_ptr<T>& object){return createNIOMessage(type,std::function<wire::Bytes()>([&]{if(!object)throw std::runtime_error("Null streamable");return object->serialise();}));}
    template<class T> requires requires(const T& object){object.serialise();}
    static void sendNetworkMessage(const std::optional<std::string>& uid,const MiniByte& type,const T& object){sendSerialized(uid,type,[&]{return object.serialise();});}
    template<class T> static void sendNetworkMessage(const std::optional<std::string>& uid,const MiniByte& type,const std::shared_ptr<T>& object){if(!object)throw wire::IOException("Cannot sendNetworkMessage with a NULL Object");sendSerialized(uid,type,[&]{return object->serialise();});}
    template<class T> static void sendNetworkMessageAll(const MiniByte& type,const T& object){sendNetworkMessage(std::string(),type,object);}
    static std::optional<Greeting> sendPingMessage(const std::optional<std::string>& host,int32_t port,bool suppressErrorMessage);
    friend class NIOManagerFixture;
};
}
