#include "QuickState.hpp"
namespace minima::p2p::testing {
namespace {
std::shared_ptr<P2PState> addressedState(){auto state=std::make_shared<P2PState>();state->setMyMinimaAddress(std::string("10.0.0.1"));state->setHostSet(true);return state;}
void known(const std::shared_ptr<P2PState>& state,const std::string& prefix){const auto addresses=QuickInetLists::generateInetSockAddrList(prefix,1,9001);for(const auto& address:*addresses)state->getKnownPeers()->add(address);}
}
std::shared_ptr<P2PState> QuickState::stateNoConnections(bool hasMinimaAddress){auto state=std::make_shared<P2PState>();known(state,"192.168.0.");if(hasMinimaAddress){state->setMyMinimaAddress(std::string("10.0.0.1"));state->setHostSet(true);}return state;}
std::shared_ptr<P2PState> QuickState::stateOutLinksOnly(int count){auto state=addressedState();state->setOutLinks(QuickInetLists::generateConnectionUIDMap("outLinkUID","70.0.0.",count,0));known(state,"192.168.0.");return state;}
std::shared_ptr<P2PState> QuickState::stateInLinksOnly(int count,int port){auto state=addressedState();state->setInLinks(QuickInetLists::generateConnectionUIDMap("inLinkUID","60.0.0.",count,port));known(state,"60.0.0.");return state;}
std::shared_ptr<P2PState> QuickState::stateInLinkKnownPeerOnly(){auto state=addressedState();known(state,"60.0.0.");state->getInLinks()->put("knownPeer",state->getKnownPeers()->elements().at(0));return state;}
std::shared_ptr<P2PState> QuickState::stateInAndOutLinks(int out,int in,int port){auto state=addressedState();state->setOutLinks(QuickInetLists::generateConnectionUIDMap("outLinkUID","70.0.0.",out,port));state->setInLinks(QuickInetLists::generateConnectionUIDMap("inLinkUID","60.0.0.",in,port));known(state,"192.168.0.");return state;}
std::shared_ptr<P2PState> QuickState::stateNoneP2PLink(int count,int port){auto state=addressedState();state->setNoneP2PLinks(QuickInetLists::generateConnectionUIDMap("noneP2PUID","10.0.1.",count,port));known(state,"192.168.0.");return state;}
std::shared_ptr<P2PState> QuickState::stateFullLinks(int count,int notAccepting,int port){auto state=addressedState();state->setOutLinks(QuickInetLists::generateConnectionUIDMap("outLinkUID","70.0.0.",count,port));state->setInLinks(QuickInetLists::generateConnectionUIDMap("inLinkUID","60.0.0.",count,port));state->setNotAcceptingConnP2PLinks(QuickInetLists::generateConnectionUIDMap("notAcceptingConnUID","50.0.1.",notAccepting,port));state->setNoneP2PLinks(QuickInetLists::generateConnectionUIDMap("noneP2PUID","10.0.1.",count,port));known(state,"192.168.0.");return state;}
}
