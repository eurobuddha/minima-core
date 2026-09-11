#include "testingutils/QuickState.hpp"
#include "org/minima/system/network/p2p/SwapLinksFunctions.hpp"
#include <iostream>
using namespace minima;using namespace minima::p2p;using namespace minima::p2p::testing;
static int assertions=0;static void require(bool value){++assertions;if(!value)throw std::runtime_error("SwapLinksFunctionsTest assertion");}
static std::shared_ptr<Message> disconnected(const std::string& uid,bool incoming){auto msg=std::make_shared<Message>();msg->addString("uid",uid).addBoolean("incoming",incoming).addBoolean("reconnect",false);return msg;}
static void testOnDisconnectedKnownPeer(){auto state=QuickState::stateInLinkKnownPeerOnly();SwapLinksFunctions::onDisconnected(state,disconnected("knownPeer",true));require(!state->getKnownPeers()->isEmpty());}
static void testOnDisconnectedInLink(){auto state=QuickState::stateInLinksOnly(1,0);SwapLinksFunctions::onDisconnected(state,disconnected("inLinkUID1",true));require(state->getInLinks()->size()==0);}
static void testOnDisconnectedOutLink(){auto state=QuickState::stateOutLinksOnly(1);SwapLinksFunctions::onDisconnected(state,disconnected("outLinkUID1",false));require(state->getOutLinks()->size()==0);}
static void testOnDisconnectedNoneP2PLink(){auto state=QuickState::stateNoneP2PLink(1,0);SwapLinksFunctions::onDisconnected(state,disconnected("noneP2PUID1",true));require(state->getNoneP2PLinks()->size()==0);}
static void greeting(bool incoming){auto client=std::make_shared<NIOClientInfo>("noneP2PUID1","10.0.1.1",9001,incoming);auto source=QuickState::stateInAndOutLinks(2,3,0);source->setMyMinimaAddress(std::string("10.0.1.1"));auto greeting=std::make_shared<P2PGreeting>(*source);auto state=QuickState::stateNoneP2PLink(1,9001);SwapLinksFunctions::processGreeting(state,greeting,client,true);require((incoming?state->getInLinks():state->getOutLinks())->size()!=0);require(state->getNoneP2PLinks()->size()==0);}
static void testProcessGreetingInLink(){greeting(true);}static void testProcessGreetingOutLink(){greeting(false);}
static void testProcessRequestIPMsg(){auto q=std::make_shared<json::JSONObject>();auto secret=MiniData::getRandomData(12).to0xString();q->put("req_ip",secret);auto response=SwapLinksFunctions::processRequestIPMsg(q,"147.258.369.1");require(std::get<std::shared_ptr<json::JSONObject>>(response.get("swap_links_p2p").value)->getString("secret")==secret);}
static void testProcessResponseIPMsgIncorrectSecret(){auto q=std::make_shared<json::JSONObject>();auto secret=MiniData::getRandomData(12);auto state=QuickState::stateInAndOutLinks(1,2,0);q->put("req_ip",secret.to0xString());auto response=SwapLinksFunctions::processRequestIPMsg(q,"147.258.369.1");SwapLinksFunctions::processResponseIPMsg(state,std::get<std::shared_ptr<json::JSONObject>>(response.get("swap_links_p2p").value));require(InetSocketAddress(std::string("10.0.0.1"),GeneralParams::MINIMA_PORT).equals(*state->getMyMinimaAddress()));}
int main(){
 // The standalone pinned JUnit process has no Main instance.
 SwapLinksFunctions::mainPresent=[]{return false;};
 for(auto test:{testOnDisconnectedKnownPeer,testOnDisconnectedInLink,testOnDisconnectedOutLink,testOnDisconnectedNoneP2PLink,testProcessGreetingInLink,testProcessGreetingOutLink,testProcessRequestIPMsg,testProcessResponseIPMsgIncorrectSecret}){
  auto inConnectMsg=std::make_shared<Message>();inConnectMsg->addString("uid","uid1").addBoolean("incoming",true);auto outConnectMsg=std::make_shared<Message>();outConnectMsg->addString("uid","uid1").addBoolean("incoming",false);test();
 }
 std::cout<<"{\"class\":\"SwapLinksFunctionsTest\",\"tests\":8,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
