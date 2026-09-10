#include "P2PDB.hpp"
namespace minima::p2p {
SocketAddressList P2PDB::getPeersList() const {
    auto result=std::make_shared<std::vector<SocketAddressPtr>>();
    if(!params()->containsKey("peers"))return result;
    auto peers=std::get<std::shared_ptr<json::JSONArray>>(params()->get("peers").value);
    if(!peers)throw std::runtime_error("Null peers array");
    if(peers->size())*result=*InetSocketAddressIO::addressesJSONToList(peers);
    return result;
}
void P2PDB::setPeersList(const SocketAddressList& peers) {params()->put("peers",InetSocketAddressIO::addressesListToJSON(peers));}
void P2PDB::setVersion() {setString("version",P2PParams::VERSION);}
std::string P2PDB::getVersion() const {return getString("version",P2PParams::VERSION);}
}
