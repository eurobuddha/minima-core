#include "WalkLinksFuncs.hpp"
#include "support/PeerOperations.hpp"
#include <algorithm>
namespace minima::p2p {
using namespace peer_detail;
namespace {
std::string text(const SocketAddressPtr& value){return value?value->toString():"null";}
std::string pathText(const SocketAddressList& path){std::string result="[";for(const auto& value:required(path)){if(result.size()>1)result+=", ";result+=text(value);}return result+"]";}
SocketAddressPtr last(const SocketAddressList& path){return required(path).at(required(path).size()-1);}
}
SocketAddressList WalkLinksFuncs::removeIPsInBFromA(const PeerMapPtr& a,const SocketAddressList& b){auto result=std::make_shared<std::vector<SocketAddressPtr>>();for(const auto& address:required(a).elements()){auto& excluded=required(b);auto& candidate=required(address);bool found=false;for(const auto& other:excluded)if(other&&candidate.equals(*other)){found=true;break;}if(!found)result->push_back(address);}return result;}
std::shared_ptr<Message> WalkLinksFuncs::onOutLinkWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& from,int32_t target,const ClientInfoList&){
    auto address=getAddressFromUID(state,required(from).getUID());required(walk).addHopToPath(address);auto filtered=removeIPsInBFromA(required(state).getOutLinks(),walk->getPathTaken());auto next=UtilFuncs::selectRandomAddress(filtered);
    if(required(walk->getPathTaken()).size()<9&&next)return createNextHopMsg(next,walk,state);return generateDoSwapMessageFromWalk(state,walk,target);
}
std::shared_ptr<Message> WalkLinksFuncs::createNextHopMsg(const SocketAddressPtr& destination,const std::shared_ptr<P2PWalkLinks>& walk,const std::shared_ptr<P2PState>& state){auto client=UtilFuncs::getClientFromInetAddress(destination,state);if(client)return send(client->getUID(),required(walk).toJson());P2PFunctions::log("[-] P2P_WALK_LINKS NIOClientInfo for "+text(destination)+" does not exist");return nullptr;}
std::shared_ptr<Message> WalkLinksFuncs::generateDoSwapMessageFromWalk(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,int32_t target){
    if(count(required(state).getInLinks())>target){auto message=std::make_shared<Message>("P2P_SEND_MSG");auto uids=required(state->getInLinks()).nullableKeys();auto uid=uids.at(size_t(random.nextInt(int32_t(uids.size())-1)));auto secret=required(walk).getSecret();auto address=required(walk->getPathTaken()).at(0);P2PDoSwap swap(secret,address,uid);message->addString("uid",uid);message->addObject("json",MessageObject(std::make_shared<json::JSONObject>(swap.toJson())));return message;}return nullptr;
}
SocketAddressPtr WalkLinksFuncs::getAddressFromUID(const std::shared_ptr<P2PState>& state,const std::optional<std::string>& uid){auto& s=required(state);auto address=get(s.getInLinks(),uid);if(!address)address=get(s.getOutLinks(),uid);if(!address)address=get(s.getNotAcceptingConnP2PLinks(),uid);if(!address)address=get(s.getNoneP2PLinks(),uid);if(!address)P2PFunctions::log("No client found for: "+uid.value_or("null"));return address;}
std::shared_ptr<Message> WalkLinksFuncs::onInLinkWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,const ClientInfoPtr& from,const ClientInfoList&){
    auto address=getAddressFromUID(state,required(from).getUID());required(walk).addHopToPath(address);auto filtered=removeIPsInBFromA(required(state).getInLinks(),walk->getPathTaken());auto next=UtilFuncs::selectRandomAddress(filtered);if(required(walk->getPathTaken()).size()<9&&next)return createNextHopMsg(next,walk,state);
    walk->addHopToPath(state->getMyMinimaAddress());walk->setReturning(true);if(walk->isClientWalk())walk->setAvailableNoneP2PConnectionSlots(std::bit_cast<int32_t>(uint32_t(state->getMaxNumNoneP2PConnections())-uint32_t(count(state->getNotAcceptingConnP2PLinks()))));return onWalkLinkResponseMsg(state,walk);
}
std::shared_ptr<Message> WalkLinksFuncs::onWalkLinkResponseMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk){auto address=required(state).getMyMinimaAddress();auto next=required(walk).getPreviousNode(address);auto client=UtilFuncs::getClientFromInetAddress(next,state);return client?send(client->getUID(),walk->toJson()):nullptr;}
WalkLinksFuncs::Messages WalkLinksFuncs::onReturnedWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk,int32_t target){
    Messages messages;if(count(required(state).getOutLinks())<target){auto path=required(walk).getPathTaken();auto address=last(path);auto own=state->getMyMinimaAddress();auto& end=required(address);if(!own||!end.equals(*own)){
        bool connected=false;while(!connected&&required(path).size()>1){auto destination=last(path);P2PFunctions::log_debug("[+] Walk to scale out-links returned. Path: "+pathText(path)+" Connecting to node: "+text(destination));auto& peer=required(destination);auto host=peer.getHostString();auto port=peer.getPort();connected=P2PFunctions::checkConnect(host,port);auto found=std::find_if(path->begin(),path->end(),[&](const auto& value){return value&&destination->equals(*value);});if(found!=path->end())path->erase(found);}
    }else P2PFunctions::log_debug("[!] P2P_WALK_LINKS_RESPONSE: Not Connecting as returned own address");}else P2PFunctions::log_debug("[!] P2P_WALK_LINKS_RESPONSE: Not Connecting already have max numLinks");return messages;
}
WalkLinksFuncs::Messages WalkLinksFuncs::onReturnedLoadBalanceWalkMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PWalkLinks>& walk){Messages messages;auto address=last(required(walk).getPathTaken());auto own=required(state).getMyMinimaAddress();auto& end=required(address);if(!own||!end.equals(*own))messages=genLoadBalanceDoSwaps(state,address,walk->getAvailableNoneP2PConnectionSlots());else P2PFunctions::log_debug("[!] P2P_WALK_LINKS_RESPONSE: Not Connecting as returned own address");return messages;}
WalkLinksFuncs::Messages WalkLinksFuncs::genLoadBalanceDoSwaps(const std::shared_ptr<P2PState>& state,const SocketAddressPtr& address,int32_t maximum){Messages messages;auto& s=required(state);if(count(s.getNotAcceptingConnP2PLinks())>s.getMaxNumNoneP2PConnections()){int32_t swaps=std::min(maximum,s.getMaxNumNoneP2PConnections()/2);for(int32_t i=0;i<swaps;++i){auto uids=required(s.getNotAcceptingConnP2PLinks()).nullableKeys();auto uid=uids.at(size_t(i));P2PDoSwap swap(std::make_shared<MiniData>(MiniData::getRandomData(8)),address,uid);messages.push_back(send(uids.at(size_t(i)),swap.toJson()));}}return messages;}
}
