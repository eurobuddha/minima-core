#include "NIOManager.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NIOConnections.hpp"
#include "support/NIOProtocol.hpp"
#include "support/JavaDateString.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include <bit>
namespace minima {
std::function<std::shared_ptr<NIOManager>()> NIOManager::runtimeLookup;
namespace {
int64_t subtract(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)-uint64_t(b));}
bool equals(const std::string& a,const std::optional<std::string>& b){return b&&unicode::decodeUTF8(a)==unicode::decodeUTF8(*b);}
std::shared_ptr<NIOClient> client(const std::shared_ptr<Message>& message){auto result=message->getObject("client").as<std::shared_ptr<NIOClient>>();if(!result)throw std::runtime_error("Null NIOClient");return result;}
std::shared_ptr<Message> clientMessage(const char* type,const std::shared_ptr<NIOClient>& value){auto msg=std::make_shared<Message>(type);msg->addObject("client",MessageObject(value));return msg;}
std::string trim(const std::string& text){size_t first=0,last=text.size();while(first<last&&uint8_t(text[first])<=32)++first;while(last>first&&uint8_t(text[last-1])<=32)--last;return text.substr(first,last-first);}
}
NIOManager::NIOManager(NIOManagerServices services):MessageProcessor("NIOMANAGER",services.releaseLocks),mServices(std::move(services)),SYNC_MAX_TIME(std::bit_cast<int64_t>(uint64_t(86400000)*uint64_t(GeneralParams::NUMBER_DAYS_ARCHIVE))){if(!mServices.currentTimeMillis)mServices.currentTimeMillis=TimerMessage::currentTimeMillis;}
std::shared_ptr<NIOManager> NIOManager::create(NIOManagerServices services){auto manager=std::shared_ptr<NIOManager>(new NIOManager(std::move(services)));manager->startServer();try{manager->startMessageProcessor();}catch(...){manager->mNIOServer->shutdown();manager->joinServer();throw;}return manager;}
void NIOManager::startServer(){std::weak_ptr<NIOManager> weak=self();mNIOServer=std::make_shared<NIOServer>(GeneralParams::MINIMA_PORT,[weak](auto message){auto owner=weak.lock();if(!owner)throw std::runtime_error("NIO manager receiver expired");owner->PostMessage(message);});auto owner=self();auto name=nextNativeThreadName();mServerThread=std::thread([owner,name=std::move(name)]()mutable{NativeThread thread{std::move(name)};runNativeThread(thread,[&]{owner->mNIOServer->run();});});}
void NIOManager::joinServer(){std::lock_guard lock(mServerJoinLock);if(mServerThread.joinable()){if(mServerThread.get_id()==std::this_thread::get_id())mServerThread.detach();else mServerThread.join();}}
NIOManager::~NIOManager(){
    mThreadPool.shutdown();
    // shutdown sets the server's stop flag before waking its selector. A wake
    // failure must not skip the ownership barriers below.
    try{if(mNIOServer)mNIOServer->shutdown();}catch(const std::exception& error){std::clog<<error.what()<<'\n';}
    stopMessageProcessor();joinServer();joinMessageProcessor();
}
std::shared_ptr<NIOManager> NIOManager::mainManager(){if(!runtimeLookup)throw JavaNullPointerException("Null Main instance");auto manager=runtimeLookup();if(!manager)throw JavaNullPointerException("Null Main NIOManager");return manager;}
std::shared_ptr<NIORuntime> NIOManager::clientRuntime(){auto runtime=std::make_shared<NIORuntime>();runtime->traffic=mTrafficListener;runtime->currentTimeMillis=mServices.currentTimeMillis;std::weak_ptr<NIOManager> weak=self();runtime->postMessage=[weak](auto message){auto owner=weak.lock();if(!owner)throw std::runtime_error("NIO manager receiver expired");owner->PostMessage(message);};return runtime;}
std::vector<NIOClientInfo> NIOManager::getAllConnectionInfo() const {std::vector<NIOClientInfo> result;for(const auto& value:mConnectingClients.elements())result.emplace_back(value,false);for(const auto& value:mNIOServer->getAllNIOClients())result.emplace_back(value,true);return result;}
std::vector<std::shared_ptr<NIOClient>> NIOManager::getAllValidConnectedClients() const {std::vector<std::shared_ptr<NIOClient>> result;for(const auto& value:mNIOServer->getAllNIOClients())if(value->isValidGreeting())result.push_back(value);return result;}
std::vector<std::shared_ptr<NIOClient>> NIOManager::getAllValidOutGoingConnectedClients() const {std::vector<std::shared_ptr<NIOClient>> result;for(const auto& value:mNIOServer->getAllNIOClients())if(value->isValidGreeting()&&value->isOutgoing())result.push_back(value);return result;}
json::JSONObject NIOManager::getAllConnectedDetails() const {int32_t incoming=0,outgoing=0;for(const auto& value:mNIOServer->getAllNIOClients())if(value->isValidGreeting()){if(value->isOutgoing())++outgoing;else ++incoming;}json::JSONObject result;result.put("total",incoming+outgoing);result.put("incoming",incoming);result.put("outgoing",outgoing);return result;}
std::shared_ptr<NIOClient> NIOManager::checkConnected(const std::optional<std::string>& host,bool onlyConnected) const {if(!onlyConnected)for(const auto& value:mConnectingClients.elements())if(equals(host.value(),value->getFullAddress()))return value;for(const auto& value:mNIOServer->getAllNIOClients())if(equals(host.value(),value->getFullAddress()))return value;return nullptr;}
std::shared_ptr<NIOClient> NIOManager::getMaximaUID(const std::optional<std::string>& key) const {for(const auto& value:mNIOServer->getAllNIOClients())if(equals(value->getMaximaIdent().value(),key))return value;return nullptr;}
std::shared_ptr<NIOClient> NIOManager::getNIOClient(const std::optional<std::string>& host) const {for(const auto& value:mNIOServer->getAllNIOClients())if(equals(value->getFullAddress(),host))return value;return nullptr;}
std::shared_ptr<NIOClient> NIOManager::getNIOClientFromUID(const std::optional<std::string>& uid) const {return mainManager()->getNIOServer()->getClient(uid);}
void NIOManager::hardShutDown(){mThreadPool.shutdown();mThreadPool.awaitTermination(8000);mNIOServer->shutdown();stopMessageProcessor();}
void NIOManager::connectAttempt(const std::shared_ptr<NIOClient>& value){
    auto owner=self();auto name=nextNativeThreadName();
    std::thread([owner,value,name=std::move(name)]()mutable{
        NativeThread thread{std::move(name)};
        runNativeThread(thread,[&]{
            try{
                auto socket=nioConnect(InetSocketAddress(value->getHost(),value->getPort()));
                owner->mConnectingClients.remove(value->getUID().value());
                owner->mNIOServer->regsiterNewSocket(socket);
                MinimaLogger::log("Connected attempt success to "+value->getFullAddress());
            }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
            catch(const std::exception& error){
                MinimaLogger::log(value->getUID().value()+" INFO : connecting attempt "+std::to_string(value->getConnectAttempts())+" to "+value->getHost().value_or("null")+":"+std::to_string(value->getPort())+" "+nativeExceptionText(error));
                owner->PostMessage(clientMessage(NIO_RECONNECT,value));
            }
        });
    }).detach();
}
void NIOManager::disconnect(std::optional<std::string> uid,bool removeP2P){
    if(GeneralParams::TXBLOCK_NODE)std::clog<<"Show Disconnect Stack Trace\n";
    if(removeP2P&&GeneralParams::P2P_ENABLED){auto value=getNIOClientFromUID(uid);if(value){mServices.addInvalidPeer(value->getFullAddress());auto address=std::make_shared<InetSocketAddress>(value->getHost(),value->getPort());auto msg=std::make_shared<Message>("P2P_REMOVE_PEER");msg->addObject("address",MessageObject(address));mainManager()->mServices.postP2P(msg);}}
    auto msg=std::make_shared<Message>(NIO_DISCONNECT);msg->addString("uid",uid);PostMessage(msg);
}
void NIOManager::processMessage(std::shared_ptr<Message> message){
    if(!message)throw std::runtime_error("Null NIO message");
    if((mServices.isShuttingDown()||mServices.isRestoring())&&message->getMessageType().value()!=NIO_SHUTDOWN)return;
    const auto& type=message->getMessageType().value();
    if(type==NIO_SERVERSTARTED){
        mServices.postP2P(std::make_shared<Message>("P2P_INIT"));
        const auto& text=GeneralParams::CONNECT_LIST;size_t start=0;while(start<text.size()){auto end=text.find(',',start);if(end==std::string::npos)end=text.size();if(end>start){auto host=trim(text.substr(start,end-start));auto msg=commands::connect::createConnectMessage(host);if(!msg)std::clog<<"ERROR connect host specified incorrectly : "<<host<<'\n';else{std::clog<<"Attempt to connect to specified host in 10 seconds : "<<host<<'\n';schedule(std::make_shared<TimerMessage>(10000,msg));}}start=end+1;}
        schedule(std::make_shared<TimerMessage>(LASTREAD_CHECKER,NIO_CHECKLASTMSG));
    }else if(type==NIO_SHUTDOWN){
        mThreadPool.shutdown();mThreadPool.awaitTermination(8000);mNIOServer->shutdown();std::clog<<"Shutdown Networking..\n";while(mNIOServer->isRunning())nativeThreadSleep(50);stopMessageProcessor();
    }else if(type==NIO_CONNECT){
        auto host=message->getString("host");int32_t port=message->getInteger("port");auto full=host.value_or("null")+":"+std::to_string(port);if(mServices.isInvalidPeer(full)){std::clog<<"NIO_CONNECT : Trying to connect to Invalid Peer - disallowed @ "<<full<<'\n';return;}
        auto value=std::make_shared<NIOClient>(host,port);mConnectingClients.put(value->getUID().value(),value);PostMessage(clientMessage(NIO_CONNECTATTEMPT,value));
    }else if(type==NIO_CONNECTATTEMPT){auto value=client(message);if(!mConnectingClients.containsKey(value->getUID().value())){std::clog<<"Connect attempt to "<<value->getFullAddress()<<" cancelled.. (removed from connecting clients)\n";return;}if(GeneralParams::TXBLOCK_NODE)std::clog<<"Slave Node Connect attempt to "<<value->getFullAddress()<<'\n';connectAttempt(value);
    }else if(type==NIO_RECONNECT){
        auto value=client(message);value->incrementConnectAttempts();bool reconnect=true,p2pSent=false;
        if(GeneralParams::TXBLOCK_NODE){if(value->getConnectAttempts()>RECONNECT_ATTEMPTS){std::clog<<"INFO : Slave node attempt reconnect.. "<<value->getFullAddress()<<'\n';value->setConnectAttempts(1);}}
        else if(!GeneralParams::P2P_ENABLED){if(value->getConnectAttempts()>RECONNECT_ATTEMPTS){std::clog<<"INFO : P2P disabled.. attempt reconnect.. "<<value->getFullAddress()<<" use disconnect to stop.\n";value->setConnectAttempts(1);}}
        else if(value->getConnectAttempts()>RECONNECT_ATTEMPTS){int connected=getNumberOfConnectedClients(),connecting=getNumberOfConnnectingClients();if(connected>0||connecting>1){reconnect=false;p2pSent=true;auto msg=clientMessage("P2P_NOCONNECT",value);msg->addString("uid",value->getUID());mServices.postP2P(msg);std::clog<<"INFO : "<<value->getUID().value()<<'@'<<value->getFullAddress()<<" connection failed - no more reconnect attempts\n";mServices.addInvalidPeer(value->getFullAddress());}else{std::clog<<"INFO : "<<value->getUID().value()<<'@'<<value->getFullAddress()<<" Resetting reconnect attempts (no other connections) for "<<value->getFullAddress()<<'\n';value->setConnectAttempts(1);}}
        if(reconnect&&mServices.isInvalidPeer(value->getFullAddress())){std::clog<<"NIO_RECONNECT : Trying to connect to Invalid Peer - disallowed @ "<<value->getFullAddress()<<'\n';reconnect=false;if(!p2pSent){auto msg=clientMessage("P2P_NOCONNECT",value);msg->addString("uid",value->getUID());mServices.postP2P(msg);}}
        if(reconnect){auto timer=std::make_shared<TimerMessage>(RECONNECT_TIMER,NIO_CONNECTATTEMPT);timer->addObject("client",MessageObject(value));schedule(timer);}else mConnectingClients.remove(value->getUID().value());
    }else if(type==NIO_DISCONNECTALL){for(const auto& value:mConnectingClients.elements())disconnect(value->getUID());for(const auto& value:mNIOServer->getAllNIOClients())disconnect(value->getUID());
    }else if(type==NIO_DISCONNECT){auto uid=message->getString("uid");mConnectingClients.remove(uid.value());mNIOServer->disconnect(uid);
    }else if(type==NIO_DISCONNECTED){
        auto value=client(message);mServices.clearClientSync(value->getUID().value());bool reconnect=message->exists("reconnect")?message->getBoolean("reconnect"):false;if(!value->isValidGreeting()||value->isIncoming())reconnect=false;if(reconnect&&mServices.isInvalidPeer(value->getFullAddress())){std::clog<<"NIO_DISCONNECT : Trying to connect to Invalid Peer - disallowed @ "<<value->getFullAddress()<<'\n';reconnect=false;}
        if(GeneralParams::TXBLOCK_NODE){std::clog<<"SLAVE NODE disconneced.. from:"<<value->getUID().value();if(message->exists("reconnect"))std::clog<<" req:"<<(message->getBoolean("reconnect")?"true":"false");std::clog<<" reconnect:"<<(reconnect?"true":"false")<<" validgreeting:"<<(value->isValidGreeting()?"true":"false")<<" host:"<<value->getFullAddress()<<" incoming:"<<(value->isIncoming()?"true":"false")<<'\n';}
        if(GeneralParams::TXBLOCK_NODE&&value->isOutgoing()&&!checkConnected(value->getFullAddress(),true)){if(!reconnect)std::clog<<"FORCE Slave reconnect "<<value->getUID().value()<<' '<<value->getFullAddress()<<'\n';reconnect=true;}
        if(reconnect&&value->isOutgoing()){auto pending=std::make_shared<NIOClient>(value->getHost(),value->getPort());mConnectingClients.put(pending->getUID().value(),pending);auto timer=std::make_shared<TimerMessage>(RECONNECT_TIMER,NIO_CONNECTATTEMPT);timer->addObject("client",MessageObject(pending));schedule(timer);}
        auto msg=std::make_shared<Message>("P2P_DISCONNECTED");msg->addObject("nioclient",MessageObject(value)).addString("uid",value->getUID()).addBoolean("incoming",value->isIncoming()).addBoolean("reconnect",reconnect);mServices.postP2P(msg);
    }else if(type==NIO_NEWCONNECTION){
        auto value=client(message);mConnectingClients.remove(value->getUID().value());if(mServices.isInvalidPeer(value->getFullAddress())){std::clog<<"Disconnecting invalid peer before sending or recieving ANY data..\n";disconnect(value->getUID());return;}
        if(value->isOutgoing()&&!value->haveSentGreeting()){value->setSentGreeting(true);Greeting greeting;greeting.createGreeting();sendNetworkMessage(value->getUID(),MiniByte(0),greeting);}
    }else if(type==NIO_INCOMINGMSG){
        auto uid=message->getString("uid");auto data=message->getObject("data");auto payload=data.isNull()?std::shared_ptr<MiniData>{}:data.as<std::shared_ptr<MiniData>>();bool trace=isTrace();auto filter=getTraceFilter();auto full=message->exists("fullhost")?message->getString("fullhost"):std::optional<std::string>("");auto owner=self();mThreadPool.execute([owner,uid,payload,trace,filter,full]{owner->mServices.processIncoming(uid,payload,trace,filter,full);});
    }else if(type==NIO_TXPOWREQ){auto id=message->getString("txpowid"),uid=message->getString("client"),reason=message->getString("reason");if(!mServices.txPoWExists(id)){std::clog<<"INFO : Requesting TxPoW "<<id.value_or("null")<<" from "<<uid.value_or("null")<<" : "<<reason.value_or("null")<<'\n';sendNetworkMessage(uid,MiniByte(3),MiniData::fromHex(id.value()));}
    }else if(type==NIO_SYNCTXBLOCK){
        auto root=mServices.getRoot();if(!root){std::clog<<"No TxPoWTree yet.. required for NIO_SYNCTXBLOCK\n";return;}auto uid=message->getString("client");auto last=mServices.loadLastArchiveBlock();if(!last){root=mServices.getRoot();if(!root)throw std::runtime_error("Null tree root");}const TxPoW& pow=last?last->getTxPoW():root->getTxPoW();if(pow.getBlockNumber().isEqual(MiniNumber::ONE))return;
        if(!GeneralParams::ARCHIVE&&pow.getTimeMilli().getAsLong()<subtract(mServices.currentTimeMillis(),SYNC_MAX_TIME)){if(GeneralParams::IBDSYNC_LOGS)std::clog<<"We have enough archive blocks.. lastblock "<<javaDateString(pow.getTimeMilli().getAsLong())<<'\n';return;}if(GeneralParams::IBDSYNC_LOGS)std::clog<<"[+] Request Sync IBD @ "<<pow.getBlockNumber().toString()<<'\n';sendNetworkMessage(uid,MiniByte(13),pow);
    }else if(type==NIO_CHECKLASTMSG){
        int64_t now=mServices.currentTimeMillis();for(const auto& value:mNIOServer->getAllNIOClients())if(subtract(now,value->getLastReadTime())>MAX_LASTREAD_CHECKER){std::clog<<"INFO : No recent message (10 mins) from "<<value->getUID().value()<<" disconnect/reconnect incoming:"<<(value->isIncoming()?"true":"false")<<" valid:"<<(value->isValidGreeting()?"true":"false")<<" host:"<<value->getFullAddress()<<'\n';disconnect(value->getUID());if(value->isOutgoing()&&value->isValidGreeting()){auto timer=std::make_shared<TimerMessage>(5000,NIO_CONNECT);timer->addString("host",value->getHost()).addInteger("port",value->getPort());schedule(timer);}}schedule(std::make_shared<TimerMessage>(LASTREAD_CHECKER,NIO_CHECKLASTMSG));
    }
    // NIO_HEALTHCHECK and unknown message types have no active body in the pin.
}
MiniData NIOManager::createNIOMessage(const MiniByte& type,const std::function<wire::Bytes()>& serializer){try{auto bytes=type.serialise();wire::append(bytes,serializer());return MiniData(std::move(bytes));}catch(const std::bad_alloc&){std::clog<<"OUT OF MEMORY.. on create NIOMsssage:"<<nioMessageType(type)<<'\n';throw wire::IOException("Out Of Memory..");}}
void NIOManager::sendSerialized(const std::optional<std::string>& uid,const MiniByte& type,const std::function<wire::Bytes()>& serializer){
    if(!serializer)throw wire::IOException("Cannot sendNetworkMessage with a NULL Object");auto data=std::make_shared<MiniData>(createNIOMessage(type,serializer));if(GeneralParams::NETWORKING_LOGS)std::clog<<"[NETLOGS SEND] to:"<<uid.value_or("null")<<" type:"<<nioMessageType(type)<<" size:"<<MiniFormat::formatSize(data->size())<<'\n';std::string howmany="single";
    if(!uid.value().empty())mainManager()->mNIOServer->sendMessage(uid,data);else{mainManager()->mNIOServer->sendMessageAll(data);howmany="all";}try{mainManager()->mTrafficListener->addWriteBytes(nioMessageType(type)+"_"+howmany,int64_t(data->size()));}catch(const std::exception&){}
}
std::optional<Greeting> NIOManager::sendPingMessage(const std::optional<std::string>& host,int32_t port,bool suppress){try{
    auto request=createNIOMessage(MiniByte(11),MiniData::ZERO_TXPOWID);auto socket=nioConnect(InetSocketAddress(host,port),10000);nioWriteAll(socket,request.serialise());mainManager()->mTrafficListener->addWriteBytes("sendPingMessage",int64_t(request.size()));auto header=nioReadAll(socket,4,10000);size_t at=0;int32_t size=std::bit_cast<int32_t>(wire::integer(header,at));if(size>MiniData::MINIMA_MAX_MINIDATA_LENGTH)throw wire::IOException("MiniData length above maximum");if(size<0)throw std::invalid_argument("Negative MiniData array size");auto response=nioReadAll(socket,size_t(size),10000);mainManager()->mTrafficListener->addReadBytes("sendPingMessage",int64_t(response.size()));socket->close();at=0;(void)MiniByte::deserialise(response,at);return Greeting::deserialise(response,at);
}catch(const std::exception& error){if(!suppress)std::clog<<"Error sending Single Ping message : "<<error.what()<<'\n';return std::nullopt;}}
}
