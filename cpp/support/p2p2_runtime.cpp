#include "P2PFunctionsFixture.hpp"
#include "org/minima/system/network/p2p2/P2P2Manager.hpp"
using namespace minima::p2p2;
namespace minima::p2p2 {
class P2P2FunctionsFixture {public:static void cache(std::shared_ptr<JavaStringSet> value){std::lock_guard lock(P2P2Functions::mLocalLock);P2P2Functions::mLocalAddresses=std::move(value);}};
class P2P2ManagerFixture : public P2P2Manager {
    P2P2ManagerFixture(P2P2ManagerServices services):P2P2Manager(std::move(services)){}
public:
    JSONArray timers;
    static auto create(P2P2ManagerServices services){auto value=std::shared_ptr<P2P2ManagerFixture>(new P2P2ManagerFixture(std::move(services)));value->initialize();return value;}
    void PostTimerMessage(const std::shared_ptr<TimerMessage>& message)override{JSONObject row;row.put("type",nullable(message->getMessageType()));row.put("delay",NIOManagerFixture::timerDelay(*message));timers.add(row);}
    void process(const std::shared_ptr<Message>& message){processMessage(message);}
    JSONArray queued(){std::lock_guard lock(mLock);JSONArray values;for(const auto& message:mMessages)values.add(nullable(message->getMessageType()));return values;}
};
}
static JSONValue run(const JSONObject& request){
    JSONArray events,posted,states;auto nio=NIOManagerFixture::create();NIOManager::runtimeLookup=[nio]{return nio;};NIOClient::runtimeLookup=[nio]{return nio->clientRuntime();};GeneralParams::ALLOW_ALL_IP=flag(request,"allow");GeneralParams::NETWORKING_LOGS=false;
    P2PFunctions::logger=[&](const auto& text){events.add("log:"+text);};P2PFunctions::openURL=[&](const auto&){events.add("available");if(flag(request,"offline"))throw wire::IOException("Offline fixture");};
    struct Reset{~Reset(){NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};P2PFunctions::logger={};P2PFunctions::openURL={};P2P2FunctionsFixture::cache(nullptr);}}reset;
    P2P2Functions::clearInvalidPeers();P2PFunctions::clearInvalidPeers();auto local=std::make_shared<JavaStringSet>();if(request.containsKey("locals"))for(const auto& item:array(request.get("locals"))->elements())local->add(string(item));P2P2FunctionsFixture::cache(local);P2PFunctionsFixture::cache(std::make_shared<JavaStringSet>());
    if(request.containsKey("invalid"))for(const auto& item:array(request.get("invalid"))->elements())P2P2Functions::mInvalidPeers.add(string(item));
    if(request.containsKey("clients"))for(const auto& raw:array(request.get("clients"))->elements()){auto row=object(raw);auto client=std::make_shared<NIOClient>(flag(*row,"incoming"),string(row->get("host")),int32_t(num(*row,"port",9001)),nullptr,std::make_shared<NativeNIOSelectionKey>([]{}));NIOClientFixture::uid(*client,row->get("uid").asString());nio->add(client,flag(*row,"connected"));}
    if(flag(request,"missingManager"))NIOManager::runtimeLookup=[]{return std::shared_ptr<NIOManager>{};};
    auto db=std::make_shared<P2P2DB>();auto old=std::make_shared<P2PDB>();if(request.containsKey("peers"))for(const auto& item:array(request.get("peers"))->elements())db->getAllKnownPeers()->push_back(string(item));if(request.containsKey("old"))*old->getAllData()=*object(request.get("old"));if(request.containsKey("data"))*db->getAllData()=*object(request.get("data"));
    int calls=0;auto access=[&](const char* name){events.add(name);if(++calls==num(request,"dbFailAt",-1))throw std::runtime_error("DB receiver failure");};P2P2ManagerServices services;services.releaseLocks=[]{};services.getP2P2DB=[&]{access("db2");return db;};services.getP2PDB=[&]{access("oldDB");return old;};auto manager=P2P2ManagerFixture::create(services);manager->NUMBER_DESIRED_CONNECTIONS=int32_t(num(request,"desired",3));manager->P2P2_LOOP_TIMER=num(request,"delay",300000);
    for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto kind=op->get("op").asString();JSONValue result="OK";try{
        if(kind=="ipv6")result=P2P2Functions::isIPv6(string(op->get("value")));
        else if(kind=="local")result=P2P2Functions::isIPLocal(string(op->get("value")));
        else if(kind=="invalid")result=P2P2Functions::isInvalidPeer(string(op->get("value")));
        else if(kind=="addInvalid")P2P2Functions::addInvalidPeer(string(op->get("value")));
        else if(kind=="clearInvalid")P2P2Functions::clearInvalidPeers();
        else if(kind=="connect")P2P2Functions::connect(string(op->get("host")),int32_t(num(*op,"port",9001)));
        else if(kind=="check")result=P2P2Functions::checkConnect(string(op->get("host")),int32_t(num(*op,"port",9001)));
        else if(kind=="disconnect")P2P2Functions::disconnect(string(op->get("uid")));
        else if(kind=="all")result=infos(P2P2Functions::getAllConnections());
        else if(kind=="connected")result=infos(P2P2Functions::getAllConnectedConnections());
        else if(kind=="info")result=info(P2P2Functions::getNIOCLientInfo(string(op->get("uid"))));
        else if(kind=="available")result=P2P2Functions::isNetAvailable();
        else if(kind=="locals")result=strings(P2P2Functions::getLocalAddresses());
        else if(kind=="localAdd"){P2P2Functions::getLocalAddresses()->add(string(op->get("value")));result=local==P2P2Functions::getLocalAddresses();}
        else if(kind=="independent"){P2PFunctions::mInvalidPeers.add(std::string("old-only"));P2PFunctions::getLocalAddresses()->add(std::string("old-only"));result=!P2P2Functions::isInvalidPeer("old-only")&&!P2P2Functions::getLocalAddresses()->contains(std::string("old-only"));}
        else if(kind=="random")result=nullable(manager->getRandomPeerFromList());
        else if(kind=="convert")manager->convertOldP2P();
        else if(kind=="choose")manager->checkWhichConnect();
        else if(kind=="shutdown")manager->shutdown();
        else if(kind=="process")manager->process(flag(*op,"nullMessage")?nullptr:std::make_shared<Message>(string(op->get("type"))));
        else throw std::runtime_error("Unknown operation");
    }catch(const std::exception&){result="ERROR";}
        auto drained=nio->drain();for(const auto& message:drained.elements())posted.add(message);JSONObject row;row.put("result",result);row.put("events",events);row.put("posted",posted);row.put("timers",manager->timers);row.put("pending",manager->queued());row.put("running",manager->isRunning());row.put("peers",*MiniUtil::convertArrayList(*db->getAllKnownPeers()));row.put("data",*db->getAllData());row.put("old",*old->getAllData());row.put("invalid",strings(std::shared_ptr<JavaStringSet>(&P2P2Functions::mInvalidPeers,[](auto*){})));states.add(JSONValue::parseWithException(row.toJSONString()));
    }return states;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line))).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
