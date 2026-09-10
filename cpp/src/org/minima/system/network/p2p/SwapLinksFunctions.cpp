#include "SwapLinksFunctions.hpp"
#include "support/PeerOperations.hpp"
namespace minima::p2p {
using namespace peer_detail;
json::JSONObject SwapLinksFunctions::wrapP2PMsg(const std::shared_ptr<json::JSONObject>& data){json::JSONObject result;json::JSONValue value;if(data)value.value=data;result.put("swap_links_p2p",std::move(value));return result;}
SwapLinksFunctions::Messages SwapLinksFunctions::onConnected(const std::shared_ptr<P2PState>& state,bool incoming,const std::shared_ptr<NIOClient>& info){
    Messages messages;auto uid=required(info).getUID();bool sendMessages=true;
    if(incoming){auto address=std::make_shared<InetSocketAddress>(info->getHost(),0);for(const auto& value:required(required(state).getNoneP2PLinks()).elements())if(value&&address->equals(*value)){auto message=std::make_shared<Message>("P2P_SEND_DISCONNECT");message->addString("uid",uid);messages.push_back(message);sendMessages=false;break;}put(state->getNoneP2PLinks(),uid,std::make_shared<InetSocketAddress>(info->getHost(),0));}
    else put(required(state).getNoneP2PLinks(),uid,std::make_shared<InetSocketAddress>(info->getHost(),info->getPort()));
    if(sendMessages){P2PGreeting greeting(*state);messages.push_back(send(uid,greeting.toJson()));if(!state->isHostSet()){json::JSONObject request;auto secret=std::make_shared<MiniData>(MiniData::getRandomData(12));state->setIpReqSecret(secret);request.put("req_ip",secret->to0xString());messages.push_back(send(uid,std::move(request)));}}
    return messages;
}
SwapLinksFunctions::Messages SwapLinksFunctions::onConnectedLoadBalanceRequest(const std::shared_ptr<P2PState>& state,const ClientInfoList&){
    Messages messages;auto& s=required(state);
    if(s.isAcceptingInLinks()&&s.getMyMinimaAddress()&&count(s.getNotAcceptingConnP2PLinks())>s.getMaxNumNoneP2PConnections()&&count(s.getInLinks())!=0){
        auto next=UtilFuncs::selectRandomAddress(values(s.getInLinks()));auto client=UtilFuncs::getClientFromInetAddress(next,state);P2PWalkLinks walk(true,false,required(client).getUID());walk.setClientWalk(true);
        int32_t maximum=s.getMaxNumP2PConnections();if(maximum==0)throw std::runtime_error("Division by zero");int32_t multiple=count(s.getInLinks())/maximum;
        for(int32_t i=0;i<multiple;++i)messages.push_back(send(client->getUID(),walk.toJson()));
    }return messages;
}
void SwapLinksFunctions::onDisconnected(const std::shared_ptr<P2PState>& state,const std::shared_ptr<Message>& message){auto uid=required(message).getString("uid");auto& s=required(state);remove(s.getInLinks(),uid);remove(s.getNotAcceptingConnP2PLinks(),uid);remove(s.getOutLinks(),uid);remove(s.getNoneP2PLinks(),uid);}
bool SwapLinksFunctions::processGreeting(const std::shared_ptr<P2PState>& state,const std::shared_ptr<P2PGreeting>& greeting,const ClientInfoPtr& client,bool noconnect){
    if(client){auto uid=client->getUID(),host=client->getHost();int32_t port=required(greeting).getMyMinimaPort();auto address=std::make_shared<InetSocketAddress>(host,port);bool addtoknown=host.value().find("127.0.0.1")==std::string::npos;(void)addtoknown;remove(required(state).getNoneP2PLinks(),uid);
        if(mainPresent()){auto manager=NIOManager::runtimeLookup();auto server=required(manager).getNIOServer();auto value=required(server).getClient(uid);if(value)value->setReceivedP2PGreeting();}
        if(greeting->isAcceptingInLinks()){if(client->isIncoming())put(state->getInLinks(),uid,address);else{put(state->getOutLinks(),uid,address);if(count(state->getOutLinks())>state->getMaxNumP2PConnections())P2PFunctions::disconnect(uid);}if(noconnect)noconnect=false;if(state->isDoingDiscoveryConnection()){state->setDoingDiscoveryConnection(false);P2PFunctions::disconnect(uid);}}
        else put(state->getNotAcceptingConnP2PLinks(),uid,address);
    }else P2PFunctions::log_debug("[-] ERROR Client is null when processing greeting: "+required(greeting).toJson().toJSONString());
    return noconnect;
}
json::JSONObject SwapLinksFunctions::processRequestIPMsg(const std::shared_ptr<json::JSONObject>& message,const std::optional<std::string>& host){auto secret=MiniData::fromHex(required(message).get("req_ip").asString());json::JSONObject response,body;body.put("res_ip",host?json::JSONValue(*host):json::JSONValue{});body.put("secret",secret.to0xString());response.put("swap_links_p2p",body);return response;}
void SwapLinksFunctions::processResponseIPMsg(const std::shared_ptr<P2PState>& state,const std::shared_ptr<json::JSONObject>& message){auto secret=MiniData::fromHex(required(message).get("secret").asString());auto& s=required(state);if(required(s.getIpReqSecret()).isEqual(secret)){auto raw=message->get("res_ip");auto host=raw.isNull()?std::optional<std::string>{}:std::optional(raw.asString());s.setMyMinimaAddress(host);s.setHostSet(true);required(s.getKnownPeers()).remove(s.getMyMinimaAddress());P2PFunctions::log_debug("[+] Setting My IP: "+host.value_or("null"));}}
SwapLinksFunctions::Messages SwapLinksFunctions::joinScaleOutLinks(const std::shared_ptr<P2PState>& state,int32_t target,const ClientInfoList&){Messages messages;auto& s=required(state);if(count(s.getOutLinks())<target){P2PFunctions::log_debug("Attempting to scale outlinks");auto next=UtilFuncs::selectRandomAddress(values(s.getOutLinks()));auto client=UtilFuncs::getClientFromInetAddress(next,state);if(client){P2PWalkLinks walk(true,true,client->getUID());messages.push_back(send(client->getUID(),walk.toJson()));}}return messages;}
SwapLinksFunctions::Messages SwapLinksFunctions::requestInLinks(const std::shared_ptr<P2PState>& state,int32_t target,const ClientInfoList&){Messages messages;auto& s=required(state);if(s.isAcceptingInLinks()&&count(s.getInLinks())<target){P2PFunctions::log_debug("Attempting to scale inlinks");auto next=UtilFuncs::selectRandomAddress(values(s.getOutLinks()));auto client=UtilFuncs::getClientFromInetAddress(next,state);if(client){P2PWalkLinks walk(false,false,client->getUID());messages.push_back(send(client->getUID(),walk.toJson()));}}return messages;}
}
