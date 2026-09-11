#pragma once
#include "QuickInetLists.hpp"
namespace minima::p2p::testing {
struct QuickState {
    static std::shared_ptr<P2PState> stateNoConnections(bool hasMinimaAddress);
    static std::shared_ptr<P2PState> stateOutLinksOnly(int numOutLinks);
    static std::shared_ptr<P2PState> stateInLinksOnly(int numInLinks,int port);
    static std::shared_ptr<P2PState> stateInLinkKnownPeerOnly();
    static std::shared_ptr<P2PState> stateInAndOutLinks(int numOutLinks,int numInLinks,int port);
    static std::shared_ptr<P2PState> stateNoneP2PLink(int numLinks,int port);
    static std::shared_ptr<P2PState> stateFullLinks(int numLinks,int numNotAcceptingLinks,int port);
};
}
