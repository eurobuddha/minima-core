#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/NIOConnections.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include <algorithm>
using namespace minima;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
template<class Predicate> static void until(Predicate predicate,const char* why){auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(!predicate()){require(std::chrono::steady_clock::now()<deadline,why);std::this_thread::sleep_for(std::chrono::milliseconds(2));}}
struct Descriptor{int value;explicit Descriptor(int fd):value(fd){}Descriptor(const Descriptor&)=delete;~Descriptor(){if(value>=0)::close(value);}int release(){int old=value;value=-1;return old;}};
struct State{
    std::mutex lock;std::condition_variable changed;
    std::weak_ptr<NIOManager> manager;
    int initialized=0,queued=0,started=0,active=0,maximum=0,cleared=0,disconnected=0;
    bool release=false,valid=true,protocol=false;
    int protocolLookups=0;
    std::optional<std::string> incomingUID;
    std::vector<int> received;
};
struct Cleanup{
    std::shared_ptr<State> state;std::shared_ptr<NIOManager> manager;
    ~Cleanup(){
        {std::lock_guard lock(state->lock);state->release=true;state->changed.notify_all();}
        if(manager){manager->hardShutDown();manager->joinMessageProcessor();}
        NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};NIOMessage::services={};commands::connect::postMessage={};Greeting::services={};
        TimerProcessor::stopTimerProcessor();TimerProcessor::getTimerProcessor()->join();
    }
};
int main(){try{
    GeneralParams::MINIMA_PORT=0;GeneralParams::P2P_ENABLED=false;GeneralParams::TXBLOCK_NODE=false;GeneralParams::CONNECT_LIST="";GeneralParams::IS_HOST_SET=false;GeneralParams::NETWORKING_LOGS=false;
    TimerProcessor::createTimerProcessor();
    auto state=std::make_shared<State>();Cleanup cleanup{state,{}};
    Greeting::services.readLock=[](bool){};Greeting::services.getWelcome=[]{return std::optional<std::string>("fixture");};Greeting::services.getTip=[]{return std::shared_ptr<TxPoWTreeNode>{};};Greeting::services.getPeersCopy=[]{return std::make_shared<std::vector<SocketAddressPtr>>();};
    NIOManagerServices services;
    services.isShuttingDown=[] {return false;};services.isRestoring=[] {return false;};
    services.releaseLocks=[state]{std::shared_ptr<NIOManager> manager;{std::lock_guard lock(state->lock);manager=state->manager.lock();}if(manager){auto message=manager->getLastMessage();if(message&&message->isMessageType(NIOManager::NIO_INCOMINGMSG)){std::lock_guard lock(state->lock);++state->queued;state->changed.notify_all();}}};
    services.postP2P=[state](auto message){std::lock_guard lock(state->lock);if(message->isMessageType("P2P_INIT"))++state->initialized;else if(message->isMessageType("P2P_DISCONNECTED")){++state->disconnected;state->valid=state->valid&&!message->getBoolean("reconnect");}state->changed.notify_all();};
    services.isInvalidPeer=[](const auto&){return false;};services.addInvalidPeer=[](const auto&){throw std::runtime_error("Unexpected invalid peer");};
    services.clearClientSync=[state](const auto&){std::lock_guard lock(state->lock);++state->cleared;};
    services.txPoWExists=[](const auto&){return false;};services.getRoot=[]{return std::shared_ptr<TxPoWTreeNode>{};};services.loadLastArchiveBlock=[]{return std::shared_ptr<TxBlock>{};};
    services.processIncoming=[state](auto uid,auto data,bool trace,const auto& filter,auto host){
        std::unique_lock lock(state->lock);
        if(state->protocol){lock.unlock();NIOMessage::dispatch(uid,data,trace,filter,host);return;}
        ++state->started;++state->active;state->maximum=std::max(state->maximum,state->active);
        state->valid=state->valid&&uid==state->incomingUID&&data&&data->size()==1&&!trace&&filter.empty()&&host&&host->starts_with("127.0.0.1:");
        state->changed.notify_all();state->changed.wait(lock,[&]{return state->release;});
        if(data&&data->size()==1)state->received.push_back(data->bytes()[0]);--state->active;state->changed.notify_all();
    };
    auto manager=NIOManager::create(services);cleanup.manager=manager;
    {std::lock_guard lock(state->lock);state->manager=manager;}
    std::weak_ptr<NIOManager> weak=manager;
    NIOManager::runtimeLookup=[weak]{return weak.lock();};NIOClient::runtimeLookup=[weak]{auto owner=weak.lock();if(!owner)throw std::runtime_error("Expired manager");return owner->clientRuntime();};
    commands::connect::postMessage=[weak](auto message){auto owner=weak.lock();if(!owner)throw std::runtime_error("Expired manager");owner->PostMessage(message);};
    until([&]{std::lock_guard lock(state->lock);return state->initialized==1;},"server startup was not delivered");
    require(manager->getNIOServer()->getBoundPort()>0,"kernel port unavailable");
    auto incoming=nioConnect(InetSocketAddress("127.0.0.1",manager->getNIOServer()->getBoundPort()),1000);
    until([&]{return manager->getNumberOfConnectedClients()==1;},"incoming connection not registered");
    auto incomingClient=manager->getNIOServer()->getAllNIOClients()[0];
    require(incomingClient->isIncoming()&&!incomingClient->haveSentGreeting()&&!incomingClient->isValidGreeting(),"incoming greeting state differs");
    {std::lock_guard lock(state->lock);state->incomingUID=incomingClient->getUID();}
    std::cout<<"incoming manager registration\n";
    Descriptor listener(::socket(AF_INET,SOCK_STREAM,0));require(listener.value>=0,"listener socket failed");sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    require(::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address))==0,"listener bind failed");require(::listen(listener.value,1)==0,"listen failed");socklen_t length=sizeof(address);require(::getsockname(listener.value,reinterpret_cast<sockaddr*>(&address),&length)==0,"listener port failed");
    commands::connect command;auto host="127.0.0.1:"+std::to_string(ntohs(address.sin_port));command.getParams()->put("host",host);command.runCommand();
    pollfd ready{listener.value,POLLIN,0};require(::poll(&ready,1,5000)==1,"manager outgoing connect timed out");Descriptor accepted(::accept(listener.value,nullptr,nullptr));require(accepted.value>=0,"accept failed");auto outgoing=std::make_shared<PosixNIOSocket>(accepted.release());
    auto header=nioReadAll(outgoing,4,5000);size_t offset=0;auto size=wire::integer(header,offset);require(size<8192,"unexpected greeting length");auto payload=nioReadAll(outgoing,size,5000);offset=0;
    require(MiniByte::deserialise(payload,offset).getValue()==0,"outgoing greeting type differs");auto greeting=Greeting::deserialise(payload,offset);
    require(offset==payload.size()&&greeting.getExtraDataValue("welcome")=="fixture"&&greeting.getExtraDataValue("port")=="0"&&greeting.getTopBlock().isEqual(MiniNumber(-1))&&greeting.getChain().empty(),"outgoing greeting contents differ");
    until([&]{return manager->getNumberOfConnectedClients()==2&&manager->getNumberOfConnnectingClients()==0;},"connected peer retained pending entry");
    auto outgoingClient=manager->checkConnected(host,true);require(outgoingClient&&outgoingClient->isOutgoing()&&outgoingClient->haveSentGreeting()&&!outgoingClient->isValidGreeting(),"outgoing manager state differs");
    require(manager->getAllValidConnectedClients().empty(),"unvalidated peer reported valid");
    outgoingClient->setValidGreeting(true);require(manager->getAllValidOutGoingConnectedClients().size()==1,"valid outgoing filter differs");require(manager->getAllConnectionInfo().size()==2,"manager info count differs");
    require(manager->getNIOClient(host)==outgoingClient&&manager->getNIOClientFromUID(outgoingClient->getUID())==outgoingClient,"manager client lookup differs");
    std::cout<<"connect command; outgoing registration; greeting wire; queries\n";
    for(int i=0;i<32;++i)nioWriteAll(incoming,MiniData(std::vector<uint8_t>{uint8_t(i)}).serialise());
    {
        std::unique_lock lock(state->lock);require(state->changed.wait_for(lock,std::chrono::seconds(5),[&]{return state->queued==32&&state->started==4;}),"incoming tasks did not queue behind four workers");
        require(state->received.empty()&&state->maximum==4,"incoming executor exceeded concurrency limit");state->release=true;state->changed.notify_all();
        require(state->changed.wait_for(lock,std::chrono::seconds(5),[&]{return state->received.size()==32;}),"incoming dispatch did not drain");
        std::sort(state->received.begin(),state->received.end());for(int i=0;i<32;++i)require(state->received[size_t(i)]==i,"incoming data lost or duplicated");require(state->valid,"incoming dispatch metadata differs");
    }
    require(manager->getTrafficListener()->getTotalRead()==32*5,"incoming frame traffic differs");
    std::cout<<"32 framed incoming tasks; four worker limit; exact metadata and traffic\n";
    NIOMessage::services.isShuttingDownOrRestoring=[]{return false;};NIOMessage::services.isSyncIBD=[]{return false;};
    NIOMessage::services.txPoWExists=[state](const auto& id){std::lock_guard lock(state->lock);require(id=="0xAABBCC","dispatcher ID lookup differs");++state->protocolLookups;return false;};
    NIOMessage::services.getTree=[]{return std::make_shared<TxPowTree>();};
    GeneralParams::P2P2_ENABLED=false;
    {std::lock_guard lock(state->lock);state->protocol=true;}
    auto announced=NIOManager::createNIOMessage(NIOMessage::MSG_TXPOWID,MiniData::fromHex("0xAABBCC"));
    nioWriteAll(incoming,announced.serialise());
    header=nioReadAll(incoming,4,5000);offset=0;size=wire::integer(header,offset);require(size<8192,"unexpected request length");payload=nioReadAll(incoming,size,5000);
    require(payload==NIOManager::createNIOMessage(NIOMessage::MSG_TXPOWREQ,MiniData::fromHex("0xAABBCC")).bytes(),"native dispatcher request wire differs");
    auto ping=NIOManager::createNIOMessage(NIOMessage::MSG_SINGLE_PING,MiniData::ZERO_TXPOWID);nioWriteAll(incoming,ping.serialise());
    header=nioReadAll(incoming,4,5000);offset=0;size=wire::integer(header,offset);require(size<8192,"unexpected pong length");payload=nioReadAll(incoming,size,5000);offset=0;
    require(MiniByte::deserialise(payload,offset).getValue()==12,"native dispatcher pong type differs");auto pong=Greeting::deserialise(payload,offset);
    require(offset==payload.size()&&pong.getExtraDataValue("welcome")=="hi there!"&&pong.getExtraDataValue("tophash")=="0x00","native dispatcher pong fields differ");
    {std::lock_guard lock(state->lock);require(state->protocolLookups==1,"native dispatcher lookup count differs");}
    require(manager->getTrafficListener()->getTotalRead()==int64_t(32*5+announced.size()+ping.size()+8),"native dispatcher framed traffic differs");
    std::cout<<"native NIOMessage dispatcher over TCP; exact TxPoW request and pong\n";
    manager->disconnect(incomingClient->getUID());
    until([&]{std::lock_guard lock(state->lock);return state->disconnected==1&&state->cleared==1;},"disconnect cleanup not delivered");
    require(manager->getNumberOfConnectedClients()==1,"disconnected peer retained");
    manager->PostMessage(NIOManager::NIO_SHUTDOWN);until([&]{return manager->isShutdownComplete();},"manager did not shut down");manager->joinMessageProcessor();
    require(!manager->getNIOServer()->isRunning(),"server still running after shutdown");
    require(nioReadAll(outgoing,0,1000).empty(),"zero read changed");bool eof=false;try{(void)nioReadAll(outgoing,1,1000);}catch(const wire::EOFException&){eof=true;}require(eof,"shutdown did not close outgoing peer");
    cleanup.manager.reset();manager.reset();until([&]{return weak.expired();},"manager ownership cycle retained shutdown runtime");
    std::cout<<"disconnect cleanup; shutdown closes sockets; manager lifetime released\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
