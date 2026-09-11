#include "testingutils/QuickState.hpp"
#include "org/minima/system/network/p2p/WalkLinksFuncs.hpp"
#include <iostream>
using namespace minima;using namespace minima::p2p;using namespace minima::p2p::testing;
static int assertions=0;static void require(bool value){++assertions;if(!value)throw std::runtime_error("WalkLinksFuncsTest assertion");}
static void testRemoveIPsInBFromA(){auto a=QuickInetLists::generateConnectionUIDMap("filter","10.0.0.",10,9001);auto b=QuickInetLists::generateInetSockAddrList("10.0.0.",5,9001);auto ret=WalkLinksFuncs::removeIPsInBFromA(a,b);require(ret->size()==5);}
static void testRemoveIPsInBFromAEmptyA(){auto a=std::make_shared<PeerMap>();auto b=QuickInetLists::generateInetSockAddrList("10.0.0.",5,9001);auto ret=WalkLinksFuncs::removeIPsInBFromA(a,b);require(ret->empty());}
static void testOnReturnedWalkMsgOutLinksFull(){auto state=QuickState::stateInAndOutLinks(5,5,9001);state->setMyMinimaAddress(std::string("60.0.0.1"));auto path=QuickInetLists::generateInetSockAddrList("60.0.0.",10,9001);auto walk=std::make_shared<P2PWalkLinks>(true,false,path);auto messages=WalkLinksFuncs::onReturnedWalkMsg(state,walk,5);require(messages.empty());}
int main(){testRemoveIPsInBFromA();testRemoveIPsInBFromAEmptyA();testOnReturnedWalkMsgOutLinksFull();std::cout<<"{\"class\":\"WalkLinksFuncsTest\",\"tests\":3,\"assertions\":"<<assertions<<",\"passed\":true}\n";}
