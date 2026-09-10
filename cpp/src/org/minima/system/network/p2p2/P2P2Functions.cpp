#include "P2P2Functions.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::p2p2 {
using p2p::P2PFunctions;
std::shared_ptr<JavaStringSet> P2P2Functions::getLocalAddresses(){
    std::lock_guard lock(mLocalLock);
    if(!mLocalAddresses){try{mLocalAddresses=enumerateInterfaces?enumerateInterfaces():p2p::localInterfaceHostnames();}
        catch(const p2p::InterfaceSocketException&){mLocalAddresses=std::make_shared<JavaStringSet>();for(const char* host:{"localhost","127.0.0.1","127.0.1.1"})mLocalAddresses->add(std::string(host));}}
    return mLocalAddresses;
}
void P2P2Functions::addInvalidPeer(const std::optional<std::string>& host){if(!mInvalidPeers.contains(host)){P2PFunctions::log("INVALID PEER added to List! "+host.value_or("null"));mInvalidPeers.add(host);}}
bool P2P2Functions::checkConnect(std::optional<std::string> host,int32_t port){
    auto message=std::make_shared<Message>(NIOManager::NIO_CONNECT);message->addString("host",host).addInteger("port",port);auto full=host.value_or("null")+":"+std::to_string(port);
    if(isInvalidPeer(full)){P2PFunctions::log("P2P2 CHECK CONNECT : Trying to connect to Invalid Peer - disallowed @ "+full);return false;}
    if(isIPv6(full)){P2PFunctions::log("P2P2 CHECK CONNECT : Trying to connect to Invalid Ipv6 Peer - disallowed @ "+full);return false;}
    try{bool local=isIPLocal(host);if(!GeneralParams::ALLOW_ALL_IP&&local){P2PFunctions::log("[!] P2P2 not connecting to local host : "+full);return false;}}
    catch(const std::bad_alloc&){throw;}catch(const std::exception&){P2PFunctions::log("[-] Error getting local addresses");}
    for(const auto& client:getAllConnections())if(!client->isConnected()&&client->getHost().value()==host&&client->getPort()==port){P2PFunctions::log("Check connect failed already attempting to connect too:"+full);return false;}
    connect(host,port);P2PFunctions::log("[!] P2P2 requesting NIO connection to: "+full);return true;
}
void P2P2Functions::main(const std::vector<std::string>&){std::cout<<"NET:"<<(isNetAvailable()?"true":"false")<<'\n';}
}
