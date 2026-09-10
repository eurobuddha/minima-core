#include "nodecount.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/network/p2p/params/P2PParams.hpp"
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/SocketAddressSet.hpp"
#include "support/JavaParseDouble.hpp"
#include "support/StateText.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cmath>
namespace minima::commands {
namespace {
// Retain PrintWriter's explicit-flush behavior and swallowed write errors.
class CsvWriter {
    int descriptor=-1;std::string pending;
public:
    explicit CsvWriter(const std::string& filename){
        auto path=MiniFile::createBaseFile(filename).string();
        if(path.find('\0')!=path.npos)throw wire::TypedIOException("java.io.FileNotFoundException","Invalid file path");
        do{descriptor=::open(path.c_str(),O_WRONLY|O_CREAT|O_TRUNC,0666);}while(descriptor<0&&errno==EINTR);
        if(descriptor<0)throw wire::TypedIOException("java.io.FileNotFoundException",path+" ("+std::strerror(errno)+")");
    }
    ~CsvWriter(){if(descriptor>=0)::close(descriptor);}
    void write(std::string text){pending+=text;}
    void flush(){
        size_t at=0;while(at<pending.size()){
            auto count=::write(descriptor,pending.data()+at,pending.size()-at);
            if(count<0&&errno==EINTR)continue;if(count<=0)break;at+=size_t(count);
        }pending.clear();
    }
};
std::shared_ptr<SocketAddressSet> unchecked(const SocketAddressSet& peers,const SocketAddressSet& pinged){
    auto result=std::make_shared<SocketAddressSet>(peers.elements());
    if(result->size()>pinged.size()){for(const auto& peer:pinged.elements())result->remove(peer);}
    else for(const auto& peer:result->elements())if(pinged.contains(peer))result->remove(peer);
    return result;
}
json::JSONObject extra(const Greeting& greeting){
    const auto& value=greeting.getExtraDataOptional();if(!value)throw JavaNullPointerException("null greeting extra data");return *value;
}
void discover(SocketAddressSet& peers,const json::JSONObject& data){
    auto value=data.get("peers-list");if(value.isNull())return;
    auto list=std::get_if<std::shared_ptr<json::JSONArray>>(&value.value);if(!list)throw JavaClassCastException("peers-list is not JSONArray");
    auto addresses=p2p::InetSocketAddressIO::addressesJSONArrayToList(*list);
    for(const auto& address:*addresses)peers.add(address);
}
std::optional<Greeting> pingPeer(const SocketAddressPtr& address){
    if(!address)throw JavaNullPointerException("null peer address");return NIOManager::sendPingMessage(address->getHostString(),address->getPort(),true);
}
int32_t javaInt(float value){return std::isnan(value)?0:double(value)>=INT32_MAX?INT32_MAX:double(value)<=INT32_MIN?INT32_MIN:int32_t(value);}
json::JSONValue jsonFloat(float value){return std::isfinite(value)?json::JSONValue::raw(javaFloating(value)):json::JSONValue();}
}
nodecount::nodecount():Command("nodecount","(file:) (complete:false|true) - Enumerate the network to count all participating nodes"){}
json::JSONValue nodecount::runCommand(){
    auto file=getParam("file","");std::unique_ptr<CsvWriter> writer;if(!file.empty())writer=std::make_unique<CsvWriter>(file);
    auto complete=getBooleanParam("complete",false);auto result=getJSONReply();json::JSONObject details;
    SocketAddressSet allPeers,pingedPeers;auto defaults=p2p::P2PParams::DEFAULT_NODE_LIST;
    if(!defaults)throw JavaNullPointerException("null default node list");
    for(const auto& address:*defaults){auto greeting=pingPeer(address);if(greeting)discover(allPeers,extra(*greeting));pingedPeers.add(address);}
    bool keepGoing=true;auto peersToCheck=unchecked(allPeers,pingedPeers);double totalClients=0;
    if(writer)writer->write("num_checked,total_peers,num_clients\n");
    float scanPercent=float(pingedPeers.size())/float(allPeers.size())/0.06f*100.0f;
    while(keepGoing){
        for(const auto& address:peersToCheck->elements()){
            auto greeting=pingPeer(address);
            if(greeting){
                auto data=extra(*greeting);auto peers=data.get("peers-list");
                if(!peers.isNull()&&!std::holds_alternative<std::shared_ptr<json::JSONArray>>(peers.value))throw JavaClassCastException("peers-list is not JSONArray");
                auto clients=data.containsKey("clients")?data.get("clients"):json::JSONValue(0.0);
                if(clients.isNull())throw JavaNullPointerException("null clients");totalClients+=javaParseDouble(send_detail::stateText(clients));discover(allPeers,data);
            }
            pingedPeers.add(address);scanPercent=float(pingedPeers.size())/float(allPeers.size())/0.06f*100.0f;
            if(writer){writer->write(std::to_string(pingedPeers.size())+","+std::to_string(allPeers.size())+","+javaFloating(totalClients)+"\n");writer->flush();}
            if(scanPercent>=100.0f&&!complete){keepGoing=false;break;}
            if(complete)scanPercent=float(pingedPeers.size())/float(allPeers.size())*100.0f;
            MinimaLogger::log("Peers Checked: "+std::to_string(pingedPeers.size())+" Scanning... "+javaFloating(scanPercent)+"%");
        }
        peersToCheck=unchecked(allPeers,pingedPeers);
        if(peersToCheck->isEmpty()||scanPercent>=100.0f){
            keepGoing=false;float clientsPerServer=float(totalClients)/float(pingedPeers.size());
            float servers=float(allPeers.size())*1.05f,clients=clientsPerServer*servers,nodes=servers+clients;
            if(complete){servers=float(allPeers.size());clients=float(totalClients);nodes=servers+clients;}
            MinimaLogger::log("Total participating nodes: "+std::to_string(javaInt(nodes))+" of which "+std::to_string(javaInt(servers))+" are p2p servers and "+std::to_string(javaInt(clients))+" are client nodes.");
            details.put("total_nodes",jsonFloat(nodes));details.put("server_nodes",jsonFloat(servers));details.put("client_nodes",jsonFloat(clients));
        }
    }
    result.put("response",details);return result;
}
}
