#include "P2PFunctions.hpp"
#include "org/minima/system/network/minima/NIOMessage.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::p2p {
std::shared_ptr<JavaStringSet> P2PFunctions::mLocalAddresses;
std::mutex P2PFunctions::mLocalLock;
std::shared_ptr<NIOManager> P2PFunctions::manager(){if(!NIOManager::runtimeLookup)throw JavaNullPointerException("Null Main instance");auto value=NIOManager::runtimeLookup();if(!value)throw JavaNullPointerException("Null NIOManager");return value;}
void P2PFunctions::log(const std::string& text){if(logger)logger(text);else std::clog<<text<<'\n';}
std::shared_ptr<JavaStringSet> P2PFunctions::getLocalAddresses(){std::lock_guard lock(mLocalLock);if(!mLocalAddresses){try{mLocalAddresses=enumerateInterfaces?enumerateInterfaces():localInterfaceHostnames();}catch(const InterfaceSocketException&){mLocalAddresses=std::make_shared<JavaStringSet>();for(const char* host:{"localhost","127.0.0.1","127.0.1.1"})mLocalAddresses->add(std::string(host));}}return mLocalAddresses;}
void P2PFunctions::addInvalidPeer(const std::optional<std::string>& host){if(!mInvalidPeers.contains(host)){log("INVALID PEER added to List! "+host.value_or("null"));mInvalidPeers.add(host);}}
bool P2PFunctions::isIPv6(const std::optional<std::string>& host){const auto& text=host.value();return text.find(':')!=text.rfind(':');}
bool P2PFunctions::isIPLocal(const std::optional<std::string>& host){const auto& text=host.value();for(const char* prefix:{"127.","localhost","10.","100.","0.","169.","172.","198.","192."})if(text.starts_with(prefix))return true;return false;}
bool P2PFunctions::isNetAvailable(){try{if(openURL)openURL("https://www.google.com");else openURLAndClose("https://www.google.com");return true;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){return false;}}
void P2PFunctions::connect(std::optional<std::string> host,int32_t port){auto message=std::make_shared<Message>(NIOManager::NIO_CONNECT);message->addString("host",host).addInteger("port",port);manager()->PostMessage(message);}
bool P2PFunctions::checkConnect(std::optional<std::string> host,int32_t port){
    auto message=std::make_shared<Message>(NIOManager::NIO_CONNECT);message->addString("host",host).addInteger("port",port);auto full=host.value_or("null")+":"+std::to_string(port);
    if(isInvalidPeer(full)){log("P2P CHECK CONNECT : Trying to connect to Invalid Peer - disallowed @ "+full);return false;}
    if(isIPv6(full)){log("P2P CHECK CONNECT : Trying to connect to Invalid Ipv6 Peer - disallowed @ "+full);return false;}
    try{bool local=isIPLocal(host);if(!GeneralParams::ALLOW_ALL_IP&&local){log_debug("[!] P2P not connecting to local host : "+full);return false;}}
    catch(const std::bad_alloc&){throw;}catch(const std::exception&){log("[-] Error getting local addresses");}
    for(const auto& client:getAllConnections())if(!client->isConnected()&&client->getHost().value()==host&&client->getPort()==port){log("Check connect failed already attempting to connect too:"+full);return false;}
    connect(host,port);log_debug("[!] P2P requesting NIO connection to: "+full);return true;
}
void P2PFunctions::disconnect(std::optional<std::string> uid){manager()->disconnect(uid);}
ClientInfoList P2PFunctions::getAllConnections(){ClientInfoList result;for(auto& value:manager()->getAllConnectionInfo())result.push_back(std::make_shared<NIOClientInfo>(std::move(value)));return result;}
ClientInfoList P2PFunctions::getAllConnectedConnections(){ClientInfoList result;for(const auto& value:getAllConnections())if(value->isConnected())result.push_back(value);return result;}
ClientInfoPtr P2PFunctions::getNIOCLientInfo(const std::optional<std::string>& uid){for(const auto& info:getAllConnections())if(info->getUID().value()==uid)return info;return nullptr;}
void P2PFunctions::sendP2PMessage(std::optional<std::string> uid,const std::shared_ptr<json::JSONObject>& message){if(!message)throw std::runtime_error("Null P2P message");MiniString text(message->toJSONString());NIOManager::sendNetworkMessage(uid,NIOMessage::MSG_P2P,text);}
void P2PFunctions::log_node_runner(const std::optional<std::string>& text){if(P2PParams::LOG_LEVEL==Level::NODE_RUNNER_MSG)log("[P2P] "+text.value_or("null"));}
void P2PFunctions::log_info(const std::optional<std::string>& text){if(P2PParams::LOG_LEVEL==Level::INFO||P2PParams::LOG_LEVEL==Level::DEBUG)log("[I] "+text.value_or("null"));}
void P2PFunctions::log_debug(const std::optional<std::string>& text){if(P2PParams::LOG_LEVEL==Level::DEBUG)log("[D] "+text.value_or("null"));}
}
