#include "P2PState.hpp"
#include <algorithm>
namespace minima::p2p {
json::JSONObject P2PState::toJson() const {
    json::JSONObject result;
    auto own=getMyMinimaAddress();auto address=own?own->toString():"not set";
    address.erase(std::remove(address.begin(),address.end(),'/'),address.end());
    result.put("address",address);result.put("is_mobile",GeneralParams::IS_MOBILE);result.put("is_accepting_connections",isAcceptingInLinks());
    auto list=[](const PeerMapPtr& map){if(!map)throw std::runtime_error("Null peer map");return InetSocketAddressIO::addressesListToJSONArray(std::make_shared<std::vector<SocketAddressPtr>>(map->elements()));};
    result.put("InLinks",list(getInLinks()));result.put("OutLinks",list(getOutLinks()));result.put("NotAcceptingConnP2PLinks",list(getNotAcceptingConnP2PLinks()));result.put("NoneP2PLinks",list(getNoneP2PLinks()));
    auto all=getAllLinks();if(!all)throw std::runtime_error("Null all links");result.put("numAllLinks",int64_t(all->size()));result.put("numKnownPeers",int64_t(getKnownPeers()->size()));return result;
}
}
