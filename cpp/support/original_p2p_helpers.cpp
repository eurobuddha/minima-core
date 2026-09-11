#include "../test/org/minima/system/network/p2p/testingutils/QuickState.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::p2p;
using namespace minima::p2p::testing;

static int integer(const JSONObject& q,const std::string& key) { return int(std::get<int64_t>(q.get(key).value)); }
static std::optional<std::string> text(const JSONObject& q,const std::string& key) {
    return q.get(key).isNull()?std::nullopt:std::optional(q.getString(key));
}
static JSONObject address(const SocketAddressPtr& value,bool randomPort=false) {
    JSONObject result;
    result.put("host",value->getHostAddress());
    result.put("resolved",!value->isUnresolved());
    result.put("port",int64_t(randomPort?-1:value->getPort()));
    result.put("portValid",randomPort?(value->getPort()>=7000&&value->getPort()<10000):(value->getPort()>=0&&value->getPort()<=65535));
    return result;
}
static JSONArray addresses(const std::vector<SocketAddressPtr>& values) {
    JSONArray result; for(const auto& value:values) result.add(address(value)); return result;
}
static JSONObject peers(const PeerMapPtr& values,bool randomPort) {
    JSONObject result;
    for(const auto& key:values->keys()) result.put(key,address(values->get(key).value(),randomPort));
    return result;
}
static JSONObject run(const JSONObject& q) {
    JSONObject result;
    try {
        const auto op=q.getString("op"); const auto count=integer(q,"count"),port=integer(q,"port");
        if(op=="list"||op=="hostname") {
            auto values=op=="list"?QuickInetLists::generateInetSockAddrList(text(q,"prefix"),count,port):
                QuickInetLists::generateInetSockAddrWithHostnameList(text(q,"hostname"),text(q,"prefix"),count,port);
            result.put("addresses",addresses(*values)); auto alias=values;
            auto retained=values->empty()?nullptr:values->front(); values->clear();
            result.put("aliasSize",int64_t(alias->size())); result.put("retained",retained?JSONValue(address(retained)):JSONValue(nullptr));
        } else if(op=="map") {
            auto values=QuickInetLists::generateConnectionUIDMap(text(q,"uid"),text(q,"prefix"),count,port);
            result.put("peers",peers(values,port<=0)); auto alias=values; values->clear(); result.put("aliasSize",int64_t(alias->size()));
        } else {
            GeneralParams::MINIMA_PORT=integer(q,"minimaPort");
            P2PParams::TGT_NUM_LINKS=17; P2PParams::TGT_NUM_NONE_P2P_LINKS=29; P2PParams::LOOP_DELAY=12345;
            const auto mode=q.getString("mode"); const auto other=integer(q,"other"); std::shared_ptr<P2PState> state;
            if(mode=="no") state=QuickState::stateNoConnections(q.getBoolean("hasAddress"));
            else if(mode=="out") state=QuickState::stateOutLinksOnly(count);
            else if(mode=="in") state=QuickState::stateInLinksOnly(count,port);
            else if(mode=="known") state=QuickState::stateInLinkKnownPeerOnly();
            else if(mode=="both") state=QuickState::stateInAndOutLinks(count,other,port);
            else if(mode=="none") state=QuickState::stateNoneP2PLink(count,port);
            else if(mode=="full") state=QuickState::stateFullLinks(count,other,port);
            else throw std::invalid_argument("unknown mode");
            result.put("own",address(state->getMyMinimaAddress())); result.put("hostSet",state->isHostSet());
            result.put("in",peers(state->getInLinks(),mode!="known"&&port<=0)); result.put("out",peers(state->getOutLinks(),mode=="out"||port<=0));
            result.put("none",peers(state->getNoneP2PLinks(),port<=0)); result.put("notAccepting",peers(state->getNotAcceptingConnP2PLinks(),port<=0));
            result.put("all",peers(state->getAllLinks(),false)); result.put("known",addresses(state->getKnownPeers()->elements()));
            result.put("accepting",state->isAcceptingInLinks()); result.put("maxP2P",int64_t(state->getMaxNumP2PConnections()));
            result.put("maxNone",int64_t(state->getMaxNumNoneP2PConnections())); result.put("delay",state->getLoopDelay());
            result.put("discovery",state->isDoingDiscoveryConnection()); result.put("startup",state->isStartupComplete());
            result.put("noConnect",state->isNoConnect()); result.put("secret",state->getIpReqSecret()->to0xString());
            auto copy=state->getKnownPeersCopy(); auto known=state->getKnownPeers();
            result.put("sameKnownPeer",mode=="known"&&state->getInLinks()->get("knownPeer").value()==copy->at(0));
            known->clear(); result.put("copyAfterClear",int64_t(copy->size())); result.put("knownAfterClear",int64_t(state->getKnownPeers()->size()));
        }
    } catch(const std::exception& error) {
        const auto value=nativeExceptionText(error); result.put("error",value.substr(0,value.find(':')));
    }
    return result;
}
int main() {
    std::cout<<std::unitbuf; std::string line;
    while(std::getline(std::cin,line)) try {
        const auto q=JSONValue::parseWithException(line);
        std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(q.value)).toJSONString()<<'\n';
    } catch(const std::exception& error) { std::cerr<<error.what()<<'\n'; return 1; }
}
