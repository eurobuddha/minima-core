#pragma once
#include "org/minima/system/network/p2p/P2PState.hpp"
namespace minima::p2p::testing {
struct QuickInetLists {
    static SocketAddressList generateInetSockAddrList(const std::optional<std::string>& prefix,int count,int port);
    static SocketAddressList generateInetSockAddrWithHostnameList(const std::optional<std::string>& hostname,const std::optional<std::string>& prefix,int count,int port);
    static PeerMapPtr generateConnectionUIDMap(const std::optional<std::string>& uidPrefix,const std::optional<std::string>& ipPrefix,int count,int port);
};
}
