// Native counterpart of the pinned InetSocketAddressIOTest.java assertions.
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
using namespace minima;using namespace minima::p2p;
int main() {
    auto peers=std::make_shared<std::vector<SocketAddressPtr>>();
    for(int i=1;i<3;++i)peers->push_back(std::make_shared<InetSocketAddress>("192.168.0."+std::to_string(i),9001));
    auto json=InetSocketAddressIO::addressesListToJSON(peers);
    if(json.toJSONString()!=R"([{"host":"192.168.0.1","port":9001},{"host":"192.168.0.2","port":9001}])")throw std::runtime_error("address list JSON differs");
    auto copy=InetSocketAddressIO::addressesJSONToList(std::make_shared<json::JSONArray>(json));
    if(copy->size()!=peers->size())throw std::runtime_error("address list size differs");
    for(size_t i=0;i<peers->size();++i)if(!peers->at(i)->equals(*copy->at(i)))throw std::runtime_error("address list round trip differs");
}
