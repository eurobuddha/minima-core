#include "testingutils/QuickInetLists.hpp"
#include "org/minima/system/network/p2p/UtilFuncs.hpp"
#include <iostream>
using namespace minima;using namespace minima::p2p;using namespace minima::p2p::testing;
static int assertions=0;static void require(bool value){++assertions;if(!value)throw std::runtime_error("UtilFuncsTest assertion");}
static void testSelectRandomAddress(){auto addresses=QuickInetLists::generateInetSockAddrList("192.168.0.",10,9001);auto address=UtilFuncs::selectRandomAddress(addresses);require(bool(address));}
static void testSelectRandomAddressEmptyList(){auto addresses=std::make_shared<std::vector<SocketAddressPtr>>();auto address=UtilFuncs::selectRandomAddress(addresses);require(!address);}
int main(){testSelectRandomAddress();testSelectRandomAddressEmptyList();std::cout<<"{\"class\":\"UtilFuncsTest\",\"tests\":2,\"assertions\":"<<assertions<<",\"passed\":true}\n";}
