#include "NetworkManager.hpp"
#include "rpc/CMDHandler.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/JavaDateString.hpp"
#include "support/PeerOperations.hpp"
#include <bit>
namespace minima {
using p2p::peer_detail::required;
namespace {
class DummyPeerManager : public MessageProcessor {
    DummyPeerManager(std::string name,std::function<void()> cleanup):MessageProcessor(std::move(name),std::move(cleanup)){}
    void processMessage(std::shared_ptr<Message> message) override{if(required(message).isMessageType(p2p::P2PFunctions::P2P_SHUTDOWN))stopMessageProcessor();}
public:
    static std::shared_ptr<DummyPeerManager> create(const std::string& name,const std::function<void()>& cleanup){auto manager=std::shared_ptr<DummyPeerManager>(new DummyPeerManager(name,cleanup));manager->startMessageProcessor();return manager;}
};
class RPCServer : public HTTPServer {
    explicit RPCServer(int32_t port):HTTPServer(port){}
public:
    static auto create(int32_t port){auto server=std::shared_ptr<RPCServer>(new RPCServer(port));server->start();return server;}
    std::function<void()> getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket) override{return [socket]{CMDHandler(socket).run();};}
};
int64_t divide(int64_t value,int64_t divisor){return value==INT64_MIN&&divisor==-1?INT64_MIN:value/divisor;}
}
std::shared_ptr<NetworkManager> NetworkManager::create(NetworkManagerServices services){auto manager=std::shared_ptr<NetworkManager>(new NetworkManager(std::move(services)));manager->initialize();return manager;}
void NetworkManager::initialize(){
    mShuttingDown=false;calculateHostIP();
    std::shared_ptr<MessageProcessor> peer;
    if(GeneralParams::P2P_ENABLED)peer=p2p::P2PManager::create(mServices.p2p);
    else peer=DummyPeerManager::create("P2P_DUMMY",mServices.nio.releaseLocks);
    {std::lock_guard lock(mOwnerLock);mP2PManager=std::move(peer);}
    auto nio=mServices.nio;std::weak_ptr<NetworkManager> weak=shared_from_this();
    // NIO's shutdown guard reads Main, not NetworkManager.mShuttingDown.
    // Preserve the supplied Main receiver independently of this owner's flag.
    nio.postP2P=[weak](auto message){auto owner=weak.lock();auto peer=required(owner).getP2PManager();required(peer).PostMessage(message);};
    nio.isInvalidPeer=[](const auto& peer){return p2p::P2PFunctions::isInvalidPeer(peer);};
    nio.addInvalidPeer=[](const auto& peer){p2p::P2PFunctions::addInvalidPeer(peer);};
    auto manager=NIOManager::create(std::move(nio));{std::lock_guard lock(mOwnerLock);mNIOManager=std::move(manager);}
    if(GeneralParams::P2P2_ENABLED)peer=p2p2::P2P2Manager::create(mServices.p2p2);
    else peer=DummyPeerManager::create("P2P2_DUMMY",mServices.nio.releaseLocks);
    {std::lock_guard lock(mOwnerLock);mP2P2Manager=std::move(peer);}
    if(GeneralParams::RPC_ENABLED)startRPC();
}
NetworkManager::~NetworkManager(){
    // Native ownership cleanup also handles construction that fails after a worker starts.
    try{stopRPC();}catch(const std::exception& error){p2p::P2PFunctions::log(error.what());}
    try{if(auto nio=getNIOManager())nio->hardShutDown();}catch(const std::exception& error){p2p::P2PFunctions::log(error.what());}
    if(auto peer=getP2PManager())peer->stopMessageProcessor();if(auto peer=getP2P2Manager())peer->stopMessageProcessor();join();
}
void NetworkManager::calculateHostIP(){
    if(GeneralParams::IS_HOST_SET)return;
    try{
        GeneralParams::MINIMA_HOST="127.0.0.1";bool found=false;
        for(const auto& iface:mServices.enumerateInterfaces()){
            if(found)break;if(iface.loopback||!iface.up)continue;
            for(const auto& address:iface.addresses)if(address.find(':')==std::string::npos){GeneralParams::MINIMA_HOST=address;if(iface.name.starts_with("wl")){found=true;break;}}
        }
    }catch(const p2p::InterfaceSocketException& error){p2p::P2PFunctions::log("ERROR calculating host IP : "+std::string(error.what()));}
}
json::JSONObject NetworkManager::getStatus(bool all){
    json::JSONObject stats;auto users=mServices.getUserDB();(void)users;
    stats.put("host",GeneralParams::MINIMA_HOST);stats.put("hostset",GeneralParams::IS_HOST_SET);stats.put("port",GeneralParams::MINIMA_PORT);
    auto nio=getNIOManager();stats.put("connecting",required(nio).getNumberOfConnnectingClients());stats.put("connected",nio->getNumberOfConnectedClients());
    json::JSONObject rpc;rpc.put("enabled",GeneralParams::RPC_ENABLED.load());rpc.put("port",GeneralParams::RPC_PORT);stats.put("rpc",rpc);if(!all)return stats;
    if(GeneralParams::P2P_ENABLED){auto peer=std::dynamic_pointer_cast<p2p::P2PManager>(getP2PManager());stats.put("p2p",required(peer).getStatus(false));}else stats.put("p2p","disabled");
    stats.put("p2p2",GeneralParams::P2P2_ENABLED?"enabled":"disabled");auto traffic=nio->getTrafficListener();
    auto duration=std::bit_cast<int64_t>(uint64_t(mServices.currentTimeMillis())-uint64_t(traffic->getStartTime()));
    // Duration stores negative milliseconds as floor seconds plus positive nanos;
    // toMinutes then divides the stored seconds with Java truncation toward zero.
    int64_t seconds=duration/1000;if(duration%1000<0)--seconds;int64_t minutes=seconds/60;if(minutes==0)minutes=1;
    json::JSONObject readwrite;readwrite.put("from",javaDateString(traffic->getStartTime()));
    readwrite.put("totalread",MiniFormat::formatSize(traffic->getTotalRead()));readwrite.put("totalwrite",MiniFormat::formatSize(traffic->getTotalWrite()));readwrite.put("breakdown",traffic->getBreakdown());
    auto read=divide(traffic->getTotalRead(),minutes),write=divide(traffic->getTotalWrite(),minutes);
    readwrite.put("read",MiniFormat::formatSize(read)+"/min");readwrite.put("write",MiniFormat::formatSize(write)+"/min");stats.put("traffic",readwrite);return stats;
}
void NetworkManager::startRPC(){std::lock_guard lock(mRPCLock);if(!mRPCServer)mRPCServer=RPCServer::create(GeneralParams::RPC_PORT);}
void NetworkManager::stopRPC(){std::lock_guard lock(mRPCLock);if(mRPCServer){mRPCServer->shutdown();mRPCServer->join();mRPCServer.reset();}}
int32_t NetworkManager::getBoundRPCPort() const{std::lock_guard lock(mRPCLock);return mRPCServer?mRPCServer->getBoundPort():0;}
void NetworkManager::shutdownNetwork(){
    mShuttingDown=true;stopRPC();auto nio=getNIOManager();required(nio).PostMessage(NIOManager::NIO_SHUTDOWN);
    auto peer=getP2PManager();if(GeneralParams::P2P_ENABLED)required(std::dynamic_pointer_cast<p2p::P2PManager>(peer)).shutdown();else required(peer).stopMessageProcessor();
    peer=getP2P2Manager();if(GeneralParams::P2P2_ENABLED)required(std::dynamic_pointer_cast<p2p2::P2P2Manager>(peer)).shutdown();else required(peer).stopMessageProcessor();
}
bool NetworkManager::isShutDownComplete() const{return required(getNIOManager()).isShutdownComplete()&&required(getP2PManager()).isShutdownComplete()&&required(getP2P2Manager()).isShutdownComplete();}
void NetworkManager::hardShutDown(){try{required(getNIOManager()).hardShutDown();}catch(const std::exception& error){p2p::P2PFunctions::log(error.what());}required(getP2PManager()).stopMessageProcessor();required(getP2P2Manager()).stopMessageProcessor();}
void NetworkManager::join(){
    if(auto nio=getNIOManager())nio->joinMessageProcessor();
    if(auto peer=getP2PManager()){peer->joinMessageProcessor();if(auto manager=std::dynamic_pointer_cast<p2p::P2PManager>(peer))if(auto checker=manager->getPeersChecker()){checker->stopMessageProcessor();checker->joinMessageProcessor();}}
    if(auto peer=getP2P2Manager())peer->joinMessageProcessor();
}
}
