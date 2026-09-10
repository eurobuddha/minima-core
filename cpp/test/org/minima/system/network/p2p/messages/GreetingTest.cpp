// Native counterpart of the pinned GreetingTest.java assertions.
#include "org/minima/system/network/p2p/messages/P2PGreeting.hpp"
using namespace minima;using namespace minima::p2p;
int main() {
    auto known=std::make_shared<std::vector<SocketAddressPtr>>(),in=std::make_shared<std::vector<SocketAddressPtr>>(),out=std::make_shared<std::vector<SocketAddressPtr>>();
    for(int i=1;i<3;++i){known->push_back(std::make_shared<InetSocketAddress>("192.168.0."+std::to_string(i),9001));in->push_back(std::make_shared<InetSocketAddress>("10.0.0."+std::to_string(i),9001));out->push_back(std::make_shared<InetSocketAddress>("10.0.0."+std::to_string(i*2),9001));}
    P2PGreeting no(9121,known),yes(9121,true,out,in,5,20,known);
    const std::string expected[]={R"({"greeting":{"myMinimaPort":9121,"isAcceptingInLinks":false,"knownPeers":[{"host":"192.168.0.1","port":9001},{"host":"192.168.0.2","port":9001}]}})",R"({"greeting":{"myMinimaPort":9121,"isAcceptingInLinks":true,"numNoneP2PConnections":5,"maxNumNoneP2PConnections":20,"outLinks":[{"host":"10.0.0.2","port":9001},{"host":"10.0.0.4","port":9001}],"inLinks":[{"host":"10.0.0.1","port":9001},{"host":"10.0.0.2","port":9001}],"knownPeers":[{"host":"192.168.0.1","port":9001},{"host":"192.168.0.2","port":9001}]}})"};
    int i=0;for(auto* greeting:{&no,&yes}){auto json=greeting->toJson();if(json.toString()!=expected[i++])throw std::runtime_error("greeting JSON differs");auto copy=P2PGreeting::fromJSON(*std::get<std::shared_ptr<json::JSONObject>>(json.get("greeting").value));if(!copy.equals(greeting))throw std::runtime_error("greeting round trip differs");}
}
