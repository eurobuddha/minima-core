#include "P2PManager.hpp"
#include "params/P2PTestParams.hpp"
#include "support/PeerOperations.hpp"
#include "support/PeerShuffle.hpp"
#include "support/JavaInteger.hpp"
#include "org/minima/system/commands/network/connect.hpp"
#include <algorithm>
#include <chrono>
namespace minima::p2p {
using namespace peer_detail;
namespace {
template<class T> std::shared_ptr<T> payload(const Message& message,const char* name){auto value=message.getObject(name);return value.isNull()?nullptr:value.as<std::shared_ptr<T>>();}
std::shared_ptr<json::JSONObject> jsonObject(const json::JSONValue& value){return value.isNull()?nullptr:std::get<std::shared_ptr<json::JSONObject>>(value.value);}
std::shared_ptr<Message> addressMessage(const char* type,const SocketAddressPtr& address){auto message=std::make_shared<Message>(type);message->addObject("address",MessageObject(address));return message;}
std::shared_ptr<Message> uidMessage(const char* type,const std::optional<std::string>& uid){auto message=std::make_shared<Message>(type);message->addString("uid",uid);return message;}
void append(P2PManager::Messages& target,P2PManager::Messages values){target.insert(target.end(),values.begin(),values.end());}
// Same String.trim and comma-token scan as NIOManager startup.
std::string trim(const std::string& text){size_t first=0,last=text.size();while(first<last&&uint8_t(text[first])<=32)++first;while(last>first&&uint8_t(text[last-1])<=32)--last;return text.substr(first,last-first);}
}
P2PManager::P2PManager(P2PManagerServices services):MessageProcessor("P2PMANAGER",services.releaseLocks),mServices(std::move(services)){}
void P2PManager::initialize(){
    if(GeneralParams::TEST_PARAMS){P2PFunctions::log("[+] P2P System using Test Params");P2PTestParams::setTestParams();}
    mPeersChecker=createPeersChecker();
    PostTimerMessage(std::make_shared<TimerMessage>(10000,P2P_LOOP));
    PostTimerMessage(std::make_shared<TimerMessage>(P2PParams::NODE_NOT_ACCEPTING_CHECK_DELAY,P2P_ASSESS_CONNECTIVITY));
    PostTimerMessage(std::make_shared<TimerMessage>(300000,P2P_SAVE_DATA));
    EXCLUDE_FROM_CLEAR={P2P_LOOP,P2P_ASSESS_CONNECTIVITY,P2P_SAVE_DATA};
}
std::shared_ptr<P2PPeersChecker> P2PManager::createPeersChecker(){return P2PPeersChecker::create(shared_from_this(),mServices.getTip,mServices.releaseLocks);}
std::shared_ptr<P2PManager> P2PManager::create(P2PManagerServices services){auto manager=std::shared_ptr<P2PManager>(new P2PManager(std::move(services)));manager->initialize();manager->startMessageProcessor();return manager;}
P2PManager::~P2PManager(){if(mPeersChecker){mPeersChecker->stopMessageProcessor();mPeersChecker->joinMessageProcessor();}}
std::string P2PManager::getP2PAddress() const{auto value=required(state->getMyMinimaAddress()).toString();value.erase(std::remove(value.begin(),value.end(),'/'),value.end());return value;}
float P2PManager::getClients() const{return float(count(state->getNoneP2PLinks()))/float(P2PParams::MIN_NUM_CONNECTIONS);}
P2PManager::Messages P2PManager::init(const std::shared_ptr<P2PState>& state){
    Messages messages;auto db=mServices.getP2PDB();auto version=required(db).getVersion();(void)version;
    P2PFunctions::log("[+] P2P Version: "+P2PParams::VERSION);auto peers=db->getPeersList();INITIAL_PEERS_LIST_NUM=int32_t(required(peers).size());P2PFunctions::log("P2P Peers found : "+std::to_string(INITIAL_PEERS_LIST_NUM));
    if(!GeneralParams::P2P_ADDNODES.empty()&&peers->empty()){
        P2PFunctions::log("Peers list empty - adding specified p2pnodes.. "+GeneralParams::P2P_ADDNODES);
        const auto& text=GeneralParams::P2P_ADDNODES;size_t start=0;while(start<text.size()){auto end=text.find(',',start);if(end==std::string::npos)end=text.size();if(end>start){auto node=trim(text.substr(start,end-start));auto checker=commands::connect::createConnectMessage(node);if(!checker)P2PFunctions::log("Invalid P2P node : "+node);else{auto host=checker->getString("host");auto port=checker->getInteger("port");peers->push_back(std::make_shared<InetSocketAddress>(host,port));}}start=end+1;}
    }
    for(const auto& peer:*peers){auto message=addressMessage(P2PPeersChecker::PEERS_CHECKPEERS,peer);message->addBoolean("force",true);required(mPeersChecker).PostMessage(message);}
    auto& s=required(state);s.setAcceptingInLinks(GeneralParams::IS_ACCEPTING_IN_LINKS);s.setMyMinimaAddress(GeneralParams::MINIMA_HOST);
    if(GeneralParams::IS_HOST_SET)s.setHostSet(true);
    s.setNoConnect(GeneralParams::NOCONNECT);s.setMaxNumNoneP2PConnections(P2PParams::TGT_NUM_NONE_P2P_LINKS);s.setMaxNumP2PConnections(s.isAcceptingInLinks()?P2PParams::TGT_NUM_LINKS:P2PParams::MIN_NUM_CONNECTIONS);
    SocketAddressPtr connectionAddress;
    if(!s.isNoConnect()){
        if(firstgo){firstgo=false;connectionAddress=std::make_shared<InetSocketAddress>("65.108.211.228",9001);P2PFunctions::log("[+] HACK Connect to wrong node: "+connectionAddress->toString());}
        else if(!GeneralParams::P2P_ROOTNODE.empty()){
            // String.split(":") retains leading/interior empty fields and drops
            // trailing empties; only the first two fields are used by Java.
            const auto& root=GeneralParams::P2P_ROOTNODE;std::vector<std::string> fields;size_t at=0;while(true){auto end=root.find(':',at);if(end==std::string::npos){fields.push_back(root.substr(at));break;}fields.push_back(root.substr(at,end-at));at=end+1;}while(!fields.empty()&&fields.back().empty())fields.pop_back();
            auto host=fields.at(0);auto port=javaParseInt(fields.at(1));connectionAddress=std::make_shared<InetSocketAddress>(host,port);required(mPeersChecker).PostMessage(addressMessage(P2PPeersChecker::PEERS_ADDPEERS,connectionAddress));P2PFunctions::log_info("[+] Connecting to specified node: "+connectionAddress->toString());
        }else if(!peers->empty()){connectionAddress=peers->at(size_t(rand.nextInt(int32_t(peers->size()))));P2PFunctions::log_info("[+] Connecting to saved node: "+(connectionAddress?connectionAddress->toString():"null"));}
        else doDiscoveryPing();
    }
    if(connectionAddress)messages.push_back(addressMessage(P2P_SEND_CONNECT,connectionAddress));return messages;
}
void P2PManager::pauseDiscovery(){std::this_thread::sleep_for(std::chrono::milliseconds(5000));}
void P2PManager::doDiscoveryPing(){
    if(required(P2PParams::DEFAULT_NODE_LIST).size()==0){auto now=mServices.currentTimeMillis();if(std::bit_cast<int64_t>(uint64_t(now)-uint64_t(mLastNotifyNoPeers))>NOTIFY_NOPEERS_TIMER){P2PFunctions::log("No default Peers found - please use command 'peers' to add a valid peer..");mLastNotifyNoPeers=now;}return;}
    int attempts=0;while(state->isDoingDiscoveryConnection()&&isRunning()){
        if(attempts>=3){P2PFunctions::log("Discovery node connection paused.. tried "+std::to_string(attempts)+" times..");state->setDoingDiscoveryConnection(false);return;}
        auto& defaults=required(P2PParams::DEFAULT_NODE_LIST);auto address=defaults.get(size_t(rand.nextInt(int32_t(defaults.size()))));auto host=required(address).getHostString();auto port=address->getPort();auto greeting=NIOManager::sendPingMessage(host,port,true);
        if(greeting){auto raw=greeting->getExtraData().get("peers-list");auto array=raw.isNull()?nullptr:std::get<std::shared_ptr<json::JSONArray>>(raw.value);if(array){auto peers=InetSocketAddressIO::addressesJSONArrayToList(array);shufflePeers(peers);for(const auto& peer:required(peers))state->getKnownPeers()->add(peer);auto verified=required(mPeersChecker).getVerifiedPeers();for(const auto& peer:*peers)required(verified).add(peer);P2PFunctions::log_info("[+] Discovery Completed");state->setDoingDiscoveryConnection(false);}}
        else try{pauseDiscovery();}catch(const std::exception&){P2PFunctions::log_debug("Wait interrupted");}
        ++attempts;
    }
}
void P2PManager::processMessage(std::shared_ptr<Message> message){
    Messages messages;auto& msg=required(message);
    if(msg.isMessageType(P2PFunctions::P2P_INIT))append(messages,init(state));
    else if(msg.isMessageType(P2PFunctions::P2P_SHUTDOWN))shutdown();
    else if(msg.isMessageType(P2P_HEALTH_CHECK)){if(getSize()>50){P2PFunctions::log("[!] P2P Message Overload - clear non-timer messages");clearExcept(EXCLUDE_FROM_CLEAR);}PostTimerMessage(std::make_shared<TimerMessage>(P2P_HEALTH_CHECK_TIMER,P2P_HEALTH_CHECK));}
    else if(msg.isMessageType(P2P_SAVE_DATA)){updateP2PPeersList();mServices.saveP2PDB();PostTimerMessage(std::make_shared<TimerMessage>(P2PParams::SAVE_DATA_DELAY,P2P_SAVE_DATA));}
    else if(msg.isMessageType(P2PFunctions::P2P_CONNECTED)){auto uid=msg.getString("uid");auto client=payload<NIOClient>(msg,"client");auto host=required(client).getHost();auto port=client->getPort();put(state->getAllLinks(),uid,std::make_shared<InetSocketAddress>(host,port));append(messages,connect(message,state));}
    else if(msg.isMessageType(P2PFunctions::P2P_DISCONNECTED)){auto uid=msg.getString("uid");remove(state->getAllLinks(),uid);SwapLinksFunctions::onDisconnected(state,message);auto& out=required(state->getOutLinks());if(uid?out.containsKey(*uid):out.containsKey(nullptr))P2PFunctions::log_debug("[-] P2P_DISCONNECTED from: "+uid.value_or("null")+" Current outlinks: "+std::to_string(out.size()));}
    else if(msg.isMessageType(P2PFunctions::P2P_MESSAGE))append(messages,processJsonMessages(message,state));
    else if(msg.isMessageType(P2P_LOOP)){append(messages,processLoop(state));PostTimerMessage(std::make_shared<TimerMessage>(state->getLoopDelay(),P2P_LOOP));}
    else if(msg.isMessageType(P2P_RESET)){P2PFunctions::log_debug("[+] P2P Reset in process");state->setAcceptingInLinks(GeneralParams::IS_ACCEPTING_IN_LINKS);state->setMyMinimaAddress(GeneralParams::MINIMA_HOST);state->setHostSet(GeneralParams::IS_HOST_SET);append(messages,processLoop(state));}
    else if(msg.isMessageType(P2P_RANDOM_CONNECT)){auto peers=state->getKnownPeers();if(peers->size()>0){auto index=rand.nextInt(int32_t(peers->size()));auto peer=peers->elements().at(size_t(index));messages.push_back(addressMessage(P2P_SEND_CONNECT,peer));}}
    else if(msg.isMessageType(P2PFunctions::P2P_NOCONNECT)){
        auto client=payload<NIOClient>(msg,"client");auto host=required(client).getHost();auto port=client->getPort();auto conn=std::make_shared<InetSocketAddress>(host,port);state->getKnownPeers()->remove(conn);P2PFunctions::log_debug("[-] Unable to connect to peer removing from peers list");
        std::vector<Message::Key> removeUIDs;auto in=state->getInLinks();auto entries=required(in).elements();bool contains=false;for(const auto& value:entries)if(value&&conn->equals(*value)){contains=true;break;}
        if(contains){auto keys=in->nullableKeys();for(size_t i=0;i<entries.size();++i)if(required(entries[i]).equals(*conn))removeUIDs.push_back(keys.at(i));for(const auto& uid:removeUIDs){remove(in,uid);put(state->getNotAcceptingConnP2PLinks(),uid,get(state->getAllLinks(),uid));}}
    }
    else if(msg.isMessageType(P2P_ASSESS_CONNECTIVITY)){append(messages,assessConnectivity(state));PostTimerMessage(std::make_shared<TimerMessage>(P2PParams::NODE_NOT_ACCEPTING_CHECK_DELAY,P2P_ASSESS_CONNECTIVITY));}
    else if(msg.isMessageType(P2P_REMOVE_PEER))state->getKnownPeers()->remove(payload<InetSocketAddress>(msg,"address"));
    else if(msg.isMessageType(P2P_ADD_PEER)){if(int32_t(state->getKnownPeers()->size())<P2PPeersChecker::MAX_VERIFIED_PEERS.load()){auto address=payload<InetSocketAddress>(msg,"address");state->getKnownPeers()->remove(address);state->getKnownPeers()->add(address);}}
    sendMessages(messages);
}
P2PManager::Messages P2PManager::assessConnectivity(const std::shared_ptr<P2PState>& state){Messages messages;auto& s=required(state);if(count(s.getInLinks())==0&&count(s.getNotAcceptingConnP2PLinks())==0&&count(s.getNoneP2PLinks())==0&&count(s.getOutLinks())!=0){s.setAcceptingInLinks(false);auto data=std::make_shared<json::JSONObject>();data->put("notAcceptingMsg",false);s.setMaxNumP2PConnections(P2PParams::MIN_NUM_CONNECTIONS);auto message=std::make_shared<Message>(P2P_SEND_MSG_TO_ALL);message->addObject("json",MessageObject(data));messages.push_back(message);}return messages;}
P2PManager::Messages P2PManager::processJsonMessages(const std::shared_ptr<Message>& message,const std::shared_ptr<P2PState>& state){
    Messages messages;auto data=payload<json::JSONObject>(required(message),"message");auto uid=message->getString("uid");auto client=P2PFunctions::getNIOCLientInfo(uid);
    if(!client){P2PFunctions::log_debug("[!] P2P NULL NioClient @ "+uid.value_or("null"));messages.push_back(uidMessage(P2P_SEND_DISCONNECT,uid));return messages;}
    auto swap=jsonObject(required(data).get("swap_links_p2p"));if(swap){
        if(swap->containsKey("greeting")){auto body=jsonObject(swap->get("greeting"));auto greeting=std::make_shared<P2PGreeting>(P2PGreeting::fromJSON(required(body)));for(const auto& address:required(greeting->getKnownPeers()))required(mPeersChecker).PostMessage(addressMessage(P2PPeersChecker::PEERS_ADDPEERS,address));auto minimaAddress=std::make_shared<InetSocketAddress>(client->getHost(),greeting->getMyMinimaPort());required(mPeersChecker).PostMessage(addressMessage(P2PPeersChecker::PEERS_ADDPEERS,minimaAddress));bool noConnect=SwapLinksFunctions::processGreeting(state,greeting,client,required(state).isNoConnect());if(!noConnect)state->setNoConnect(false);}
        if(swap->containsKey("req_ip")){auto response=SwapLinksFunctions::processRequestIPMsg(swap,client->getHost());P2PFunctions::sendP2PMessage(uid,std::make_shared<json::JSONObject>(std::move(response)));}
        if(swap->containsKey("res_ip"))SwapLinksFunctions::processResponseIPMsg(state,swap);
        if(swap->containsKey("notAcceptingMsg")){auto& s=required(state);remove(s.getInLinks(),uid);remove(s.getOutLinks(),uid);remove(s.getNotAcceptingConnP2PLinks(),uid);remove(s.getNoneP2PLinks(),uid);put(s.getNotAcceptingConnP2PLinks(),uid,get(s.getAllLinks(),uid));}
        if(swap->containsKey("walk_links"))append(messages,processWalkLinksMsg(swap,client,state));
        if(swap->containsKey("do_swap")){auto doSwap=P2PDoSwap::readFromJson(*swap);messages.push_back(addressMessage(P2P_SEND_CONNECT,doSwap.getSwapTarget()));messages.push_back(uidMessage(P2P_SEND_DISCONNECT,uid));}
    }return messages;
}
P2PManager::Messages P2PManager::processLoop(const std::shared_ptr<P2PState>& state){
    Messages messages;auto& s=required(state);if(count(s.getOutLinks())>=s.getMaxNumP2PConnections())s.setLoopDelay(int64_t(P2PParams::LOOP_DELAY)+rand.nextInt(P2PParams::LOOP_DELAY_VARIABILITY));else s.setLoopDelay(int64_t(10000)+rand.nextInt(3000));s.getKnownPeers()->remove(s.getMyMinimaAddress());
    if(!s.isNoConnect()){
        if(!s.getKnownPeers()->isEmpty()){
            if(count(s.getOutLinks())<1){auto peers=s.getKnownPeers();auto index=rand.nextInt(int32_t(peers->size()));messages.push_back(addressMessage(P2P_SEND_CONNECT,peers->elements().at(size_t(index))));}
            else if(count(s.getOutLinks())<s.getMaxNumP2PConnections())append(messages,SwapLinksFunctions::joinScaleOutLinks(state,s.getMaxNumP2PConnections(),P2PFunctions::getAllConnections()));
            else if(s.isAcceptingInLinks()){append(messages,SwapLinksFunctions::requestInLinks(state,s.getMaxNumP2PConnections(),P2PFunctions::getAllConnections()));append(messages,SwapLinksFunctions::onConnectedLoadBalanceRequest(state,P2PFunctions::getAllConnections()));}
        }else{
            if(count(s.getAllLinks())==0&&s.isStartupComplete())P2PFunctions::log_node_runner("[!] Node is not connected to the network. Attempting to join the network again now. Please check your not has an internet connection.");
            if(s.getKnownPeers()->size()==0&&!s.isStartupComplete()){auto db=mServices.getP2PDB();auto peers=required(db).getPeersList();for(const auto& peer:required(peers))required(mPeersChecker).PostMessage(addressMessage(P2PPeersChecker::PEERS_ADDPEERS,peer));}
            doDiscoveryPing();
        }
    }return messages;
}
P2PManager::Messages P2PManager::processWalkLinksMsg(const std::shared_ptr<json::JSONObject>& message,const ClientInfoPtr& client,const std::shared_ptr<P2PState>& state){auto walk=std::make_shared<P2PWalkLinks>(P2PWalkLinks::readFromJSON(required(message)));if(walk->isReturning())return processReturningMessage(walk,state);return {processOutgoingWalkMessage(walk,client,state)};}
P2PManager::Messages P2PManager::processReturningMessage(const std::shared_ptr<P2PWalkLinks>& walk,const std::shared_ptr<P2PState>& state){auto own=required(state).getMyMinimaAddress();auto first=required(required(walk).getPathTaken()).at(0);auto& address=required(own);if(first&&address.equals(*first)){if(walk->isClientWalk())return WalkLinksFuncs::onReturnedLoadBalanceWalkMsg(state,walk);return WalkLinksFuncs::onReturnedWalkMsg(state,walk,state->getMaxNumP2PConnections());}return {WalkLinksFuncs::onWalkLinkResponseMsg(state,walk)};}
std::shared_ptr<Message> P2PManager::processOutgoingWalkMessage(const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& client,const std::shared_ptr<P2PState>& state){if(required(walk).isWalkInLinks())return WalkLinksFuncs::onInLinkWalkMsg(state,walk,client,P2PFunctions::getAllConnections());auto target=required(state).getMaxNumP2PConnections();return WalkLinksFuncs::onOutLinkWalkMsg(state,walk,client,target,P2PFunctions::getAllConnections());}
P2PManager::Messages P2PManager::connect(const std::shared_ptr<Message>& message,const std::shared_ptr<P2PState>& state){auto info=payload<NIOClient>(required(message),"client");auto messages=SwapLinksFunctions::onConnected(state,required(info).isIncoming(),info);append(messages,SwapLinksFunctions::onConnectedLoadBalanceRequest(state,P2PFunctions::getAllConnections()));return messages;}
json::JSONObject P2PManager::getStatus(bool fullDetails){
    int32_t inbound=0,outbound=0;for(const auto& info:P2PFunctions::getAllConnections())if(info->isConnected()){if(info->isIncoming())++inbound;else ++outbound;}
    json::JSONObject result;if(fullDetails){result.put("p2p_state",state->toJson());result.put("numUnvalidatedPeers",int64_t(required(mPeersChecker).getUnverifiedPeers()->size()));}
    else{result.put("address",getP2PAddress());result.put("isAcceptingInLinks",state->isAcceptingInLinks());result.put("numInLinks",count(state->getInLinks()));result.put("numOutLinks",count(state->getOutLinks()));result.put("numNotAcceptingConnP2PLinks",count(state->getNotAcceptingConnP2PLinks()));result.put("numNoneP2PLinks",count(state->getNoneP2PLinks()));result.put("numKnownPeers",int64_t(state->getKnownPeers()->size()));result.put("numUnvalidatedPeers",int64_t(required(mPeersChecker).getUnverifiedPeers()->size()));result.put("numAllLinks",count(state->getAllLinks()));result.put("nio_inbound",inbound);result.put("nio_outbound",outbound);}return result;
}
void P2PManager::sendMessages(const Messages& messages){for(const auto& message:messages)if(message){if(message->isMessageType(P2P_SEND_CONNECT)){auto address=payload<InetSocketAddress>(*message,"address");auto host=required(address).getHostString();auto port=address->getPort();P2PFunctions::checkConnect(host,port);}else if(message->isMessageType(P2P_SEND_DISCONNECT))P2PFunctions::disconnect(message->getString("uid"));else if(message->isMessageType(P2P_SEND_MSG)){auto uid=message->getString("uid");auto wrapped=SwapLinksFunctions::wrapP2PMsg(payload<json::JSONObject>(*message,"json"));P2PFunctions::sendP2PMessage(uid,std::make_shared<json::JSONObject>(std::move(wrapped)));}else if(message->isMessageType(P2P_SEND_MSG_TO_ALL)){auto wrapped=SwapLinksFunctions::wrapP2PMsg(payload<json::JSONObject>(*message,"json"));P2PFunctions::sendP2PMessageAll(std::make_shared<json::JSONObject>(std::move(wrapped)));}}}
void P2PManager::updateP2PPeersList(){auto db=mServices.getP2PDB();auto size=int32_t(state->getKnownPeers()->size());if(size>0&&size>=INITIAL_PEERS_LIST_NUM/2)required(db).setPeersList(std::make_shared<std::vector<SocketAddressPtr>>(state->getKnownPeers()->elements()));}
void P2PManager::shutdown(){auto db=mServices.getP2PDB();required(db).setVersion();updateP2PPeersList();required(mPeersChecker).stopMessageProcessor();P2PFunctions::log("P2PDB shutdown..");stopMessageProcessor();}
bool P2PManager::haveAnyPeers(){if(required(P2PParams::DEFAULT_NODE_LIST).size()>0)return true;if(required(mPeersChecker).haveAnyPeers())return true;auto db=mServices.getP2PDB();if(required(required(db).getPeersList()).size()>0)return true;return state->getKnownPeers()->size()>0;}
}
