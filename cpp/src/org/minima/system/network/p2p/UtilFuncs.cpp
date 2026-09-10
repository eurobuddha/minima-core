#include "UtilFuncs.hpp"
namespace minima::p2p {
ClientInfoPtr UtilFuncs::searchLinksMapForAddress(const SocketAddressPtr& address,const PeerMapPtr& links){
    if(!links)throw std::runtime_error("Null links map");
    for(const auto& uid:links->nullableKeys()){auto value=uid?links->get(*uid):links->get(nullptr);if(!value||!*value)throw std::runtime_error("Null link address");if(address&&(*value)->equals(*address))return P2PFunctions::getNIOCLientInfo(uid);}
    return nullptr;
}
ClientInfoPtr UtilFuncs::getClientFromInetAddress(const SocketAddressPtr& address,const std::shared_ptr<P2PState>& state){
    if(!state)throw std::runtime_error("Null P2P state");auto info=searchLinksMapForAddress(address,state->getInLinks());
    if(!info)info=searchLinksMapForAddress(address,state->getOutLinks());if(!info)info=searchLinksMapForAddress(address,state->getNotAcceptingConnP2PLinks());if(!info)info=searchLinksMapForAddress(address,state->getNoneP2PLinks());return info;
}
SocketAddressPtr UtilFuncs::selectRandomAddress(const SocketAddressList& addresses){if(!addresses)throw std::runtime_error("Null address list");if(addresses->empty())return nullptr;int32_t index=0;if(addresses->size()>1){if(addresses->size()>INT32_MAX)throw std::length_error("Java list size exceeded");index=rnd.nextInt(int32_t(addresses->size()));}return addresses->at(size_t(index));}
}
