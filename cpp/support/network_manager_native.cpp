#include "org/minima/system/network/NetworkManager.hpp"
#include "org/minima/system/network/rpc/Authorizer.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/NIOConnections.hpp"
#include <array>
#include <semaphore>
using namespace minima;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
template<class Predicate> static void until(Predicate predicate,const char* why){auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(!predicate()){require(std::chrono::steady_clock::now()<end,why);std::this_thread::sleep_for(std::chrono::milliseconds(2));}}
struct Cleanup {
    std::shared_ptr<NetworkManager> network;
    ~Cleanup(){if(network){network->hardShutDown();network->stopRPC();network->join();network.reset();}NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};NIOMessage::services={};commands::connect::postMessage={};Authorizer::userDB={};p2p::P2PFunctions::logger={};p2p::P2PFunctions::openURL={};TimerProcessor::stopTimerProcessor();if(auto timer=TimerProcessor::getTimerProcessor())timer->join();}
};
static std::string rpc(int32_t port){
    auto socket=nioConnect(InetSocketAddress("127.0.0.1",port),1000);std::string request="GET /hash+data:network-manager HTTP/1.1\r\n\r\n";
    nioWriteAll(socket,{reinterpret_cast<const uint8_t*>(request.data()),request.size()});std::string result;std::array<uint8_t,4096> buffer{};
    for(;;){auto count=nioReadSome(socket,buffer,5000);if(count<0)break;result.append(reinterpret_cast<const char*>(buffer.data()),size_t(count));}return result;
}
static void exercise(bool p2pEnabled,bool p2p2Enabled,bool hard){
    GeneralParams::MINIMA_PORT=0;GeneralParams::RPC_PORT=0;GeneralParams::MINIMA_HOST="127.0.0.1";GeneralParams::IS_HOST_SET=true;
    GeneralParams::P2P_ENABLED=p2pEnabled;GeneralParams::P2P2_ENABLED=p2p2Enabled;GeneralParams::RPC_ENABLED=true;GeneralParams::RPC_AUTHENTICATE=false;GeneralParams::RPC_CRLF=true;
    GeneralParams::TXBLOCK_NODE=false;GeneralParams::CONNECT_LIST="";GeneralParams::NOCONNECT=true;GeneralParams::TEST_PARAMS=false;GeneralParams::P2P_ADDNODES="";GeneralParams::P2P_ROOTNODE="";GeneralParams::NETWORKING_LOGS=false;
    p2p::P2PParams::DEFAULT_NODE_LIST=std::make_shared<JavaList<SocketAddressPtr>>();
    p2p::P2PFunctions::logger=[](const auto&){};p2p::P2PFunctions::openURL=[](const auto&){throw wire::IOException("Offline fixture");};
    auto users=std::make_shared<UserDB>();auto db=std::make_shared<p2p::P2PDB>();auto db2=std::make_shared<p2p2::P2P2DB>();auto tree=std::make_shared<TxPowTree>();
    auto completions=std::make_shared<std::counting_semaphore<1024>>(0);
    auto mainShutdown=std::make_shared<std::atomic<bool>>(false);
    NetworkManagerServices services;services.nio.releaseLocks=[completions]{completions->release();};services.nio.isShuttingDown=[mainShutdown]{return mainShutdown->load();};services.nio.isRestoring=[]{return false;};services.nio.clearClientSync=[](const auto&){};
    services.nio.processIncoming=[](auto uid,auto data,bool trace,auto filter,auto host){NIOMessage::dispatch(uid,data,trace,filter,host);};services.nio.txPoWExists=[](const auto&){return false;};services.nio.getRoot=[tree]{return tree->getRoot();};services.nio.loadLastArchiveBlock=[]{return std::shared_ptr<TxBlock>{};};
    services.p2p.releaseLocks=services.nio.releaseLocks;services.p2p.getP2PDB=[db]{return db;};services.p2p.saveP2PDB=[]{throw std::runtime_error("Unexpected persistence timer");};services.p2p.getTip=[tree]{return tree->getTip();};
    services.p2p2.releaseLocks=services.nio.releaseLocks;services.p2p2.getP2PDB=[db]{return db;};services.p2p2.getP2P2DB=[db2]{return db2;};services.getUserDB=[users]{return users;};
    Authorizer::userDB=services.getUserDB;NIOMessage::services.isShuttingDownOrRestoring=[]{return false;};NIOMessage::services.isSyncIBD=[]{return false;};NIOMessage::services.getTree=[tree]{return tree;};
    TimerProcessor::createTimerProcessor();Cleanup cleanup;cleanup.network=NetworkManager::create(services);auto network=cleanup.network;std::weak_ptr<NetworkManager> weak=network;
    NIOManager::runtimeLookup=[weak]{auto owner=weak.lock();return owner?owner->getNIOManager():nullptr;};NIOClient::runtimeLookup=[weak]{auto owner=weak.lock();if(!owner)throw std::runtime_error("Expired network");return owner->getNIOManager()->clientRuntime();};
    NIOMessage::services.getPeersCopy=[weak]{return std::dynamic_pointer_cast<p2p::P2PManager>(weak.lock()->getP2PManager())->getPeersCopy();};
    NIOMessage::services.getP2PClients=[weak]{return std::dynamic_pointer_cast<p2p::P2PManager>(weak.lock()->getP2PManager())->getClients();};
    NIOMessage::services.getP2P2Peers=[db2]{return db2->getAllKnownPeers();};
    auto nio=network->getNIOManager();auto peer=network->getP2PManager();auto peer2=network->getP2P2Manager();
    until([&]{auto last=peer->getLastMessage();return last&&last->isMessageType("P2P_INIT");},"NIO startup did not reach P2P worker");
    // A barrier message is processed after INIT on the actual peer queue. The
    // source's unknown-message body is empty; cleanup semaphore confirms work.
    peer->PostMessage("NETWORK_FIXTURE_BARRIER");until([&]{auto last=peer->getLastMessage();return last&&last->isMessageType("NETWORK_FIXTURE_BARRIER");},"Peer initialization barrier stalled");
    if(p2p2Enabled){until([&]{auto last=peer2->getLastMessage();return last&&last->isMessageType(p2p2::P2P2Manager::P2P2_INIT);},"P2P2 initialization missing");peer2->PostMessage("NETWORK_FIXTURE_BARRIER");until([&]{auto last=peer2->getLastMessage();return last&&last->isMessageType("NETWORK_FIXTURE_BARRIER");},"P2P2 initialization barrier stalled");}
    require(completions->try_acquire_for(std::chrono::seconds(5)),"Database cleanup callback was never used");
    require(bool(std::dynamic_pointer_cast<p2p::P2PManager>(peer))==p2pEnabled,"Wrong P2P manager type");require(bool(std::dynamic_pointer_cast<p2p2::P2P2Manager>(peer2))==p2p2Enabled,"Wrong P2P2 manager type");
    until([&]{return network->getBoundRPCPort()>0;},"RPC listener did not bind");auto first=network->getBoundRPCPort();network->startRPC();require(first==network->getBoundRPCPort(),"startRPC replaced live listener");
    auto response=rpc(first);require(response.starts_with("HTTP/1.1 200 OK\r\n")&&response.find("\"status\":true")!=std::string::npos&&response.find("network-manager")!=std::string::npos,"Actual network RPC command failed");
    network->stopRPC();network->stopRPC();require(network->getBoundRPCPort()==0&&GeneralParams::RPC_ENABLED,"stopRPC changed configuration");network->startRPC();until([&]{return network->getBoundRPCPort()>0;},"RPC restart failed");require(rpc(network->getBoundRPCPort()).find("\"status\":true")!=std::string::npos,"RPC command after restart failed");
    auto incoming=nioConnect(InetSocketAddress("127.0.0.1",nio->getNIOServer()->getBoundPort()),1000);auto ping=NIOManager::createNIOMessage(NIOMessage::MSG_SINGLE_PING,MiniData::ZERO_TXPOWID);nioWriteAll(incoming,ping.serialise());
    auto header=nioReadAll(incoming,4,5000);size_t offset=0;auto size=wire::integer(header,offset);require(size<8192,"Unexpected pong length");auto payload=nioReadAll(incoming,size,5000);offset=0;require(MiniByte::deserialise(payload,offset).getValue()==12,"Network dispatcher did not send pong");auto greeting=Greeting::deserialise(payload,offset);require(offset==payload.size()&&greeting.getExtraDataValue("tophash")=="0x00","Pong content mismatch");
    auto status=network->getStatus(true);require(std::get<int64_t>(status.get("connected").value)==1,"Network connection count differs");require(status.get("p2p2").asString()==(p2p2Enabled?"enabled":"disabled"),"P2P2 status differs");require(status.containsKey("traffic")&&status.containsKey("rpc"),"Full network status missing fields");if(!p2pEnabled)require(status.get("p2p").asString()=="disabled","Disabled peer status differs");
    require(nio->getTrafficListener()->getTotalRead()==int64_t(ping.size()+4),"Network traffic count differs");require(!network->isShutDownComplete(),"Live network reports shutdown");
    // Main's shutdown flag is a distinct receiver. The port prevents a socket
    // attempt if this guard regresses, while an incorrect dispatch still inserts
    // a pending client and makes the assertion fail.
    auto guardedConnect=std::make_shared<Message>(NIOManager::NIO_CONNECT);guardedConnect->addString("host","127.0.0.1").addInteger("port",-1);
    *mainShutdown=true;nio->PostMessage(guardedConnect);nio->PostMessage("NETWORK_MAIN_GUARD_BARRIER");
    until([&]{auto last=nio->getLastMessage();return last&&last->isMessageType("NETWORK_MAIN_GUARD_BARRIER");},"Main shutdown guard barrier stalled");require(nio->getNumberOfConnnectingClients()==0&&!network->mShuttingDown,"Main and network shutdown state conflated");*mainShutdown=false;
    if(hard){network->hardShutDown();require(!network->mShuttingDown,"hardShutDown changes source flag");require(network->getBoundRPCPort()>0,"hardShutDown unexpectedly stopped RPC");network->stopRPC();}else{network->shutdownNetwork();require(network->mShuttingDown&&network->getBoundRPCPort()==0,"Normal shutdown state differs");}
    network->join();require(network->isShutDownComplete(),"Network workers remain running");until([&]{return !nio->getNIOServer()->isRunning();},"NIO server remains running");
    bool eof=false;try{nioReadAll(incoming,1,1000);}catch(const wire::EOFException&){eof=true;}require(eof,"Network shutdown did not close client");
    if(p2p2Enabled)require(db2->getAllKnownPeers()->size()==2&&db2->isFirstStartUp(),"P2P2 startup DB effects differ");
    std::weak_ptr<NIOManager> weakNio=nio;std::weak_ptr<MessageProcessor> weakPeer=peer,weakPeer2=peer2;nio.reset();peer.reset();peer2.reset();cleanup.network.reset();network.reset();until([&]{return weak.expired()&&weakNio.expired()&&weakPeer.expired()&&weakPeer2.expired();},"Network ownership was retained after shutdown");
}
int main(){try{for(bool peer:{false,true})for(bool peer2:{false,true})for(bool hard:{false,true})exercise(peer,peer2,hard);std::cout<<"Eight real network lifecycles: P2P/P2P2 combinations, native RPC/restart, NIO dispatcher ping, traffic, soft/hard shutdown and owner release passed\n";}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
