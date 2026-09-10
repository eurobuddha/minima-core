#pragma once
#include "P2PFunctions.hpp"
#include "P2PState.hpp"
#include "support/JavaRandom48.hpp"
namespace minima::p2p {
class UtilFuncs {
    inline static JavaRandom48 rnd;
    UtilFuncs()=delete;
    friend class P2PFunctionsFixture;
public:
    static ClientInfoPtr searchLinksMapForAddress(const SocketAddressPtr& address,const PeerMapPtr& links);
    static ClientInfoPtr getClientFromInetAddress(const SocketAddressPtr& address,const std::shared_ptr<P2PState>& state);
    static SocketAddressPtr selectRandomAddress(const SocketAddressList& addresses);
};
}
