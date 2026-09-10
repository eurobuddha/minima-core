#include "P2PPeersChecker.hpp"
#include "support/PeerOperations.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::p2p {
using namespace peer_detail;
namespace {
std::shared_ptr<Message> addressMessage(const char* type,const SocketAddressPtr& address){auto message=std::make_shared<Message>(type);message->addObject("address",MessageObject(address));return message;}
SocketAddressPtr messageAddress(const Message& message){auto value=message.getObject("address");return value.isNull()?nullptr:value.as<SocketAddressPtr>();}
}
P2PPeersChecker::P2PPeersChecker(const std::shared_ptr<MessageStack>& manager,std::function<std::shared_ptr<TxPoWTreeNode>()> tip,std::function<void()> releaseLocks)
    :MessageProcessor("PEERS_CHECKER",std::move(releaseLocks)),p2PManager(manager),mTip(std::move(tip)){setFullLogging(false,"");PostMessage(PEERS_INIT);}
std::shared_ptr<P2PPeersChecker> P2PPeersChecker::create(const std::shared_ptr<MessageStack>& manager,std::function<std::shared_ptr<TxPoWTreeNode>()> tip,std::function<void()> releaseLocks){auto checker=std::shared_ptr<P2PPeersChecker>(new P2PPeersChecker(manager,std::move(tip),std::move(releaseLocks)));checker->startMessageProcessor();return checker;}
void P2PPeersChecker::postParent(const std::shared_ptr<Message>& message){required(p2PManager.lock()).PostMessage(message);}
std::optional<Greeting> P2PPeersChecker::ping(const SocketAddressPtr& address){auto& peer=required(address);auto host=peer.getHostString();auto port=peer.getPort();return NIOManager::sendPingMessage(host,port,true);}
void P2PPeersChecker::checkUnverifiedPeer(const SocketAddressPtr& address){
    if(unverifiedPeers->contains(address)||verifiedPeers->contains(address))return;
    if(int32_t(unverifiedPeers->size())<MAX_VERIFIED_PEERS.load()){
        if(int32_t(verifiedPeers->size())>=MAX_VERIFIED_PEERS.load()&&newRandomInt(100)<90)return;
        unverifiedPeers->add(address);PostMessage(addressMessage(PEERS_CHECKPEERS,address));
    }
}
SocketAddressPtr P2PPeersChecker::removeRandomItem(const std::shared_ptr<SocketAddressSet>& set){auto size=required(set).size();if(size==0)return nullptr;auto index=newRandomInt(int32_t(size));SocketAddressPtr chosen;int32_t i=0;for(const auto& address:set->elements()){if(i==index){chosen=address;break;}++i;}if(chosen){set->remove(chosen);return chosen;}return nullptr;}
void P2PPeersChecker::processMessage(std::shared_ptr<Message> message){
    const auto type=required(message).getMessageType().value();
    if(type==PEERS_INIT)return;
    if(type==PEERS_ADDPEERS){auto address=messageAddress(*message);auto local=P2PFunctions::getLocalAddresses();(void)local;auto host=required(address).getHostString();bool islocal=P2PFunctions::isIPLocal(host),ipv6=P2PFunctions::isIPv6(host);if(GeneralParams::ALLOW_ALL_IP||(!islocal&&!ipv6))checkUnverifiedPeer(address);else P2PFunctions::log_debug("[-] Prevent node from adding localhost / ipv6 address to peers list "+address->getHostString());}
    else if(type==PEERS_CHECKPEERS){
        auto address=messageAddress(*message);bool force=message->exists("force")?message->getBoolean("force"):false;bool forcelog=message->exists("forcelog")?message->getBoolean("forcelog"):false;
        if(force||!P2PFunctions::getAllConnectedConnections().empty()){
            auto greeting=ping(address);bool valid=false;
            if(greeting){auto version=greeting->getVersion().toString();bool test=version.find("TEST")!=std::string::npos;if(GeneralParams::TEST_PARAMS==test){valid=true;auto& extra=greeting->getExtraData();auto block=extra.getString("50block","");auto hash=extra.getString("50hash","");
                if(!block.empty()&&!hash.empty()){
                    auto tip=mTip();if(tip){auto number=MiniNumber(block);if(tip->getTxPoW().getBlockNumber().isLess(number)){if(forcelog||GeneralParams::PEERSCHECKER_lOG)P2PFunctions::log("[-] Can't check peer as tip below check block tip:"+tip->getTxPoW().getBlockNumber().toString()+" check:"+block);}
                        else {auto check=tip->getPastNode(MiniNumber(block));if(check){if(check->getTxBlock().getTxPoW().getTxPoWID()!=hash){if(forcelog||GeneralParams::PEERSCHECKER_lOG)P2PFunctions::log("PEERS CHECKER incorrect chain! @ "+block+" "+required(address).toString()+" ");valid=false;}}
                            else{if(forcelog||GeneralParams::PEERSCHECKER_lOG)P2PFunctions::log("PEERS CHECKER incorrect chain! ( null checknode ) @ "+block+" "+required(address).toString()+" ");valid=false;}}
                    }else if(forcelog||GeneralParams::PEERSCHECKER_lOG)P2PFunctions::log("[-] Can't check peer as we have no block data");
                }else{if(forcelog||GeneralParams::PEERSCHECKER_lOG)P2PFunctions::log("PEERS CHECKER no block data @ "+required(address).toString());valid=false;}
                if(valid&&forcelog)P2PFunctions::log("FORCECHECK - VALID CHAIN "+required(address).toString());
            }}else if(forcelog)P2PFunctions::log("NULL greeting from Peer "+required(address).toString());
            if(valid){unverifiedPeers->remove(address);if(int32_t(verifiedPeers->size())>=MAX_VERIFIED_PEERS.load()){auto removed=removeRandomItem(verifiedPeers);if(removed)postParent(addressMessage("P2P_REMOVE_PEER",removed));}verifiedPeers->remove(address);verifiedPeers->add(address);postParent(addressMessage("P2P_ADD_PEER",address));}
            else{if(verifiedPeers->contains(address)){verifiedPeers->remove(address);if(verifiedPeers->size()==0)P2PFunctions::log_node_runner("[-] All addresses removed from verified peers list - Check node has internet connection");unverifiedPeers->add(address);auto timer=std::make_shared<TimerMessage>(1800000,PEERS_CHECKPEERS);timer->addObject("address",MessageObject(address));PostTimerMessage(timer);}else unverifiedPeers->remove(address);postParent(addressMessage("P2P_REMOVE_PEER",address));}
        }else{auto timer=std::make_shared<TimerMessage>(60000,PEERS_CHECKPEERS);timer->addObject("address",MessageObject(address));PostTimerMessage(timer);}
    }
    else if(type==PEERS_LOOP){if(P2PFunctions::isNetAvailable())for(const auto& address:verifiedPeers->elements())PostMessage(addressMessage(PEERS_CHECKPEERS,address));PostTimerMessage(std::make_shared<TimerMessage>(PEERS_LOOP_TIMER,PEERS_LOOP));}
    else if(type==PEERS_FORCEFULLCHECK){if(P2PFunctions::isNetAvailable())for(const auto& address:verifiedPeers->elements()){auto next=addressMessage(PEERS_CHECKPEERS,address);next->addBoolean("forcelog",true);PostMessage(next);}else P2PFunctions::log("No Network connection - cannot perform peers check");}
}
}
