#include "peers.hpp"
#include "connect.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include "support/PeerShuffle.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::commands {
namespace {
std::shared_ptr<p2p::P2PManager> manager(){
    auto network=sqlRequired(Main::getInstance()).getNetworkManager();
    auto base=sqlRequired(network).getP2PManager();auto result=std::dynamic_pointer_cast<p2p::P2PManager>(base);
    if(base&&!result)throw JavaClassCastException("manager is not a P2PManager");return result;
}
bool disabled(){return !GeneralParams::P2P_ENABLED||sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).isSlaveNode();}
int32_t splitCount(std::string text){
    if(text.empty())return 1;
    while(!text.empty()&&text.back()==',')text.pop_back();
    if(text.empty())return 0;return int32_t(std::count(text.begin(),text.end(),','))+1;
}
std::pair<int32_t,int32_t> queuePeers(const std::shared_ptr<p2p::P2PPeersChecker>& checker,const std::string& text){
    int32_t valid=0,invalid=0;size_t start=0;
    while(start<text.size()){
        auto end=text.find(',',start);if(end==std::string::npos)end=text.size();
        if(end>start){auto peer=text.substr(start,end-start);auto connection=connect::createConnectMessage(peer);
            if(connection){auto address=std::make_shared<InetSocketAddress>(connection->getString("host"),connection->getInteger("port"));
                auto message=std::make_shared<Message>(p2p::P2PPeersChecker::PEERS_CHECKPEERS);message->addObject("address",MessageObject(address)).addBoolean("force",true);
                sqlRequired(checker).PostMessage(message);++valid;
            }else ++invalid;
        }start=end+1;
    }return {valid,invalid};
}
}
peers::peers():Command("peers","(action:list|addpeers) (peerslist:) - Prints the peers list this node has"){}
std::optional<std::string> peers::getFullHelp() const {return std::string(
"\npeers\n"
"\n"
"Prints the peers list this node has. P2P must be enabled.\n"
"\n"
"Your peers are the other Minima nodes you know about.\n"
"\n"
"action: (optional)\n"
"    list : List your peers. The default.\n"
"    addpeers : Add a list of new peers. \n"
"\n"
"peerslist: (optional)\n"
"    CSV list of new peers [ip:port,ip:port,..]\n"
"\n"
"Examples:\n"
"\n"
"peers\n"
"\n"
"peers action:list\n"
"\n"
"peers action:addpeers peerslist:31.125.188.214:9001,94.0.239.117:9001\n");}
std::string peers::getPeersList(int32_t max){
    std::string result;if(disabled())return result;
    try{
        auto owner=manager();auto list=sqlRequired(owner).getPeersCopy();p2p::shufflePeers(list);int64_t counter=0;
        for(const auto& peer:sqlRequired(list)){
            if(counter>max)break;
            auto& address=sqlRequired(peer);if(address.isUnresolved())throw JavaNullPointerException("null resolved peer address");
            result+=address.getHostAddress()+":"+std::to_string(address.getPort())+",";++counter;
        }
        if(result.ends_with(','))result.pop_back();
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}
    catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    return result;
}
json::JSONValue peers::runCommand(){
    auto result=getJSONReply();json::JSONObject response;
    if(disabled()){
        response.put("peers-list","");response.put("havepeers",true);response.put("p2penabled",GeneralParams::P2P_ENABLED);response.put("message","P2P System NOT enabled");result.put("response",response);return result;
    }
    auto action=getParam("action","list");
    if(action=="list"){
        auto list=getPeersList(getNumberParam("max",MiniNumber::THOUSAND).getAsInt());auto count=splitCount(list);if(javaStringTrim(list).empty())count=0;
        auto owner=manager();response.put("peerslist",list);response.put("size",count);response.put("havepeers",sqlRequired(owner).haveAnyPeers());response.put("p2penabled",GeneralParams::P2P_ENABLED);
    }else if(action=="forcecheck"){
        auto owner=manager();auto checker=sqlRequired(owner).getPeersChecker();sqlRequired(checker).PostMessage(p2p::P2PPeersChecker::PEERS_FORCEFULLCHECK);
        auto list=getPeersList(getNumberParam("max",MiniNumber::THOUSAND).getAsInt());response.put("message","Peers check started.. will start ASAP");response.put("logs",GeneralParams::PEERSCHECKER_lOG.load());response.put("size",splitCount(list));
    }else if(action=="addpeers"){
        auto owner=manager();auto checker=sqlRequired(owner).getPeersChecker();auto text=getParam("peerslist");
        if(text.starts_with("http")){auto downloaded=RPCClient::sendGET(text);if(downloaded.empty())throw CommandException("No peers found @ "+text);text=downloaded;}
        auto [valid,invalid]=queuePeers(checker,text);response.put("valid",valid);response.put("invalid",invalid);response.put("message","Valid peers added to checking queue..");
    }else if(action=="publish"){
        auto file=getParam("file","peerslist.txt");auto list=getPeersList(20);auto path=MiniFile::createBaseFile(file);
        MiniFile::writeDataToFile(path,std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(list.data()),list.size()));
        response.put("peers",list);response.put("file",javaUnixAbsoluteFile(path));
    }else if(action=="fetch"){
        if(existsParam("file"))throw CommandException("Cannot read peers from local files..");auto url=getParam("url");auto text=RPCClient::sendGET(url);
        if(text.empty())throw CommandException("No peers found in location "+url);
        auto owner=manager();auto checker=sqlRequired(owner).getPeersChecker();auto [valid,invalid]=queuePeers(checker,text);
        response.put("peers",valid);response.put("location",url);response.put("message","Valid peers added to checking queue..");
    }else throw CommandException("Invalid action : "+action);
    result.put("response",response);return result;
}
}
