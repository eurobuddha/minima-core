#include "P2PGreeting.hpp"
#include "org/minima/system/network/p2p/P2PState.hpp"
#include "support/PeerShuffle.hpp"
namespace minima::p2p {
P2PGreeting::P2PGreeting(int32_t port,SocketAddressList knownPeers):mMyMinimaPort(port),mKnownPeers(std::move(knownPeers)){}
P2PGreeting::P2PGreeting(int32_t port,bool accepting,SocketAddressList outLinks,SocketAddressList inLinks,int32_t count,int32_t maximum,SocketAddressList knownPeers):mMyMinimaPort(port),mAcceptingInLinks(accepting),mOutLinks(std::move(outLinks)),mInLinks(std::move(inLinks)),mNumNoneP2PConnections(count),mMaxNumNoneP2PConnections(maximum),mKnownPeers(std::move(knownPeers)){}
P2PGreeting::P2PGreeting(const P2PState& state) {
    mMyMinimaPort=GeneralParams::MINIMA_PORT;mAcceptingInLinks=state.isAcceptingInLinks();
    auto copy=[](const PeerMapPtr& map){if(!map)throw std::runtime_error("Null peer map");return std::make_shared<std::vector<SocketAddressPtr>>(map->elements());};
    mOutLinks=copy(state.getOutLinks());mInLinks=copy(state.getInLinks());
    if(!state.getNoneP2PLinks())throw std::runtime_error("Null non-P2P links");
    mNumNoneP2PConnections=int32_t(state.getNoneP2PLinks()->size());mMaxNumNoneP2PConnections=state.getMaxNumNoneP2PConnections();mKnownPeers=state.getKnownPeersCopy();
    if(state.getMyMinimaAddress()&&state.isAcceptingInLinks())mKnownPeers->push_back(state.getMyMinimaAddress());
    shufflePeers(mKnownPeers);
}
P2PGreeting P2PGreeting::fromJSON(const json::JSONObject& object) {
    P2PGreeting result;
    result.setMyMinimaPort(InetSocketAddressIO::safeReadInt(object,"myMinimaPort"));
    result.setAcceptingInLinks(object.containsKey("isAcceptingInLinks")?object.get("isAcceptingInLinks").asBool():false);
    auto list=[&](const char* name){return object.containsKey(name)?InetSocketAddressIO::addressesJSONToList(std::get<std::shared_ptr<json::JSONArray>>(object.get(name).value)):std::make_shared<std::vector<SocketAddressPtr>>();};
    result.setKnownPeers(list("knownPeers"));
    if(result.isAcceptingInLinks()){result.setOutLinks(list("outLinks"));result.setInLinks(list("inLinks"));result.setNumNoneP2PConnections(InetSocketAddressIO::safeReadInt(object,"numNoneP2PConnections"));result.setMaxNumNoneP2PConnections(InetSocketAddressIO::safeReadInt(object,"maxNumNoneP2PConnections"));}
    return result;
}
json::JSONObject P2PGreeting::toJson() const {
    json::JSONObject contents,result;
    contents.put("myMinimaPort",mMyMinimaPort);contents.put("isAcceptingInLinks",mAcceptingInLinks);
    if(mAcceptingInLinks){contents.put("numNoneP2PConnections",mNumNoneP2PConnections);contents.put("maxNumNoneP2PConnections",mMaxNumNoneP2PConnections);contents.put("outLinks",InetSocketAddressIO::addressesListToJSON(mOutLinks));contents.put("inLinks",InetSocketAddressIO::addressesListToJSON(mInLinks));}
    contents.put("knownPeers",InetSocketAddressIO::addressesListToJSON(mKnownPeers));result.put("greeting",contents);return result;
}
bool P2PGreeting::equals(const P2PGreeting* other) const {
    if(other==this)return true;if(!other)return false;
    if(mMyMinimaPort!=other->mMyMinimaPort||mAcceptingInLinks!=other->mAcceptingInLinks)return false;
    if(!mKnownPeers||!other->mKnownPeers)throw std::runtime_error("Null known peers");return mKnownPeers->size()==other->mKnownPeers->size();
}
}
