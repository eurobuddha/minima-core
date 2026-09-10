#include "PeerHarness.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <set>
#include <algorithm>
using Client=std::shared_ptr<NIOClient>;
static int64_t number(const JSONObject& value,const std::string& key,int64_t fallback=0){return value.containsKey(key)?std::get<int64_t>(value.get(key).value):fallback;}
static bool flag(const JSONObject& value,const std::string& key){return std::holds_alternative<bool>(value.get(key).value)&&value.get(key).asBool();}
struct FixtureState {
    std::vector<std::pair<std::string,Client>> clients;
    std::set<std::string> invalid,sync,chainSync;
    std::vector<std::shared_ptr<Message>> p2p;
    JSONArray calls;
    std::string failure;
    bool shutdown=false,restore=false,exists=false,disappear=false;
    int rootReads=0;
    std::shared_ptr<TxPoWTreeNode> root;
    std::shared_ptr<TxBlock> archive;
    void check(const std::string& stage){calls.add(stage);if(failure==stage)throw std::runtime_error("Injected "+stage);}
    Client find(const std::string& label){for(const auto& [name,value]:clients)if(name==label)return value;throw std::runtime_error("Unknown client "+label);}
    std::string label(const Client& value){if(!value)return "NULL";for(const auto& [name,client]:clients)if(value==client)return name;auto name="g"+std::to_string(clients.size());clients.emplace_back(name,value);return name;}
    std::optional<std::string> uid(const std::optional<std::string>& value){if(!value)return {};for(const auto& [name,client]:clients)if(client->getUID()==value)return name;return value;}
};
namespace minima {
struct NIOClientFixture {
    static void uid(NIOClient& client,const std::string& uid){client.mUID=uid;}
    static void last(NIOClient& client,int64_t value){client.mLastMessageRead=value;}
    static JSONValue sent(NIOClient& client){if(!client.mMessages)return {};JSONArray out;for(const auto& data:*client.mMessages)out.add(data?JSONValue(data->to0xString()):JSONValue{});return out;}
};
class NIOManagerFixture : public NIOManager {
    static NIOManagerServices services(const std::shared_ptr<FixtureState>& state){
        NIOManagerServices result;result.releaseLocks=[]{};
        result.isShuttingDown=[state]{return state->shutdown;};result.isRestoring=[state]{return state->restore;};
        result.postP2P=[state](auto message){state->p2p.push_back(message);state->check("p2p");};
        result.isInvalidPeer=[state](const auto& host){return state->invalid.contains(host);};
        result.addInvalidPeer=[state](const auto& host){state->invalid.insert(host);};
        result.clearClientSync=[state](const auto& uid){state->sync.erase(uid);state->chainSync.erase(uid);};
        result.txPoWExists=[state](const auto&){state->check("exists");return state->exists;};
        result.getRoot=[state]{state->check("root");return state->disappear&&++state->rootReads==2?nullptr:state->root;};
        result.loadLastArchiveBlock=[state]{state->check("archive");return state->archive;};
        result.currentTimeMillis=TimerMessage::currentTimeMillis;
        return result;
    }
    explicit NIOManagerFixture(std::shared_ptr<FixtureState> value):NIOManager(services(value)),state(std::move(value)){}
    void schedule(const std::shared_ptr<TimerMessage>& message) override{if(message->exists("client"))state->label(message->getObject("client").as<Client>());timers.push_back(message);state->check("timer");}
    void connectAttempt(const Client&) override{throw std::runtime_error("Fixture must not start connector");}
public:
    std::shared_ptr<FixtureState> state;
    std::vector<std::shared_ptr<TimerMessage>> timers;
    std::vector<std::shared_ptr<Message>> posted;
    static std::shared_ptr<NIOManagerFixture> create(const std::shared_ptr<FixtureState>& state){auto result=std::shared_ptr<NIOManagerFixture>(new NIOManagerFixture(state));result->mNIOServer=std::make_shared<NIOServer>(0,std::make_shared<MessageStack>());return result;}
    void invoke(const std::shared_ptr<Message>& message){processMessage(message);}
    void drain(){while(auto message=getNextMessage()){if(message->exists("client"))state->label(message->getObject("client").as<Client>());posted.push_back(message);}}
    void map(const Client& value,const std::string& which,bool add){auto& target=which=="pending"?mConnectingClients:mNIOServer->mClients;if(add)target.put(value->getUID().value(),value);else target.remove(value->getUID().value());}
    JSONValue message(const std::shared_ptr<Message>& value){
        JSONObject out;out.put("type",nullable(value->getMessageType()));
        for(const char* key:{"uid","host"})if(value->exists(key))out.put(key,nullable(std::string(key)=="uid"?state->uid(value->getString(key)):value->getString(key)));
        if(value->exists("port"))out.put("port",value->getInteger("port"));
        for(const char* key:{"client","nioclient"})if(value->exists(key)){auto raw=value->getObject(key);out.put(key,raw.isNull()?JSONValue{}:JSONValue(state->label(raw.as<Client>())));}
        for(const char* key:{"incoming","reconnect"})if(value->exists(key))out.put(key,value->getBoolean(key));
        if(value->exists("address"))out.put("address",::snapshot(value->getObject("address").as<SocketAddressPtr>()));
        return out;
    }
    JSONArray labels(const std::vector<Client>& values){std::vector<std::string> sorted;for(const auto& value:values)sorted.push_back(state->label(value));std::sort(sorted.begin(),sorted.end());JSONArray out;for(const auto& value:sorted)out.add(value);return out;}
    JSONValue snapshot(){
        drain();JSONObject out;out.put("pending",labels(mConnectingClients.elements()));out.put("connected",labels(mNIOServer->getAllNIOClients()));
        JSONArray clients;for(const auto& [name,value]:state->clients){JSONObject row;row.put("label",name);row.put("host",nullable(value->getHost()));row.put("port",value->getPort());row.put("minimaport",value->getMinimaPort());row.put("incoming",value->isIncoming());row.put("valid",value->isValidGreeting());row.put("sent",value->haveSentGreeting());row.put("attempts",value->getConnectAttempts());row.put("maxima",nullable(value->getMaximaIdent()));row.put("packets",NIOClientFixture::sent(*value));row.put("sync",state->sync.contains(value->getUID().value()));row.put("chainSync",state->chainSync.contains(value->getUID().value()));clients.add(row);}out.put("clients",clients);
        JSONArray queue,peer,scheduled,invalid,disconnects;for(const auto& value:posted)queue.add(message(value));for(const auto& value:state->p2p)peer.add(message(value));for(const auto& value:timers){auto row=object(message(value));row->put("delay",value->mDelay);scheduled.add(*row);}for(const auto& value:state->invalid)invalid.add(value);for(const auto& value:mNIOServer->mDisconnectChannels)disconnects.add(nullable(state->uid(value)));
        out.put("posted",queue);out.put("p2p",peer);out.put("timers",scheduled);out.put("invalid",invalid);out.put("disconnects",disconnects);out.put("calls",state->calls);out.put("running",isRunning());out.put("serverShutdown",mNIOServer->mShutDown.load());out.put("traffic",getTrafficListener()->getBreakdown());out.put("counts",getAllConnectedDetails());out.put("valid",labels(getAllValidConnectedClients()));out.put("outgoing",labels(getAllValidOutGoingConnectedClients()));return out;
    }
};
}
static TxPoW pow(const JSONObject& config){TxPoW value;value.clearBody();value.setBlockNumber(MiniNumber(number(config,"height",2)));value.setTimeMilli(MiniNumber(number(config,"time")));value.calculateTXPOWID();return value;}
static JSONValue run(const JSONObject& request){
    GeneralParams::P2P_ENABLED=true;GeneralParams::TXBLOCK_NODE=false;GeneralParams::ARCHIVE=false;GeneralParams::CONNECT_LIST="";GeneralParams::MINIMA_PORT=0;GeneralParams::IS_HOST_SET=false;GeneralParams::NUMBER_DAYS_ARCHIVE=50;
    auto state=std::make_shared<FixtureState>();auto manager=NIOManagerFixture::create(state);NIOManager::runtimeLookup=[manager]{return manager;};NIOClient::runtimeLookup=[manager]{return manager->clientRuntime();};
    Greeting::services.readLock=[state](bool lock){state->check(lock?"lock":"unlock");};Greeting::services.getWelcome=[]{return std::optional<std::string>("fixture");};Greeting::services.getTip=[]{return std::shared_ptr<TxPoWTreeNode>{};};Greeting::services.getPeersCopy=[]{return std::make_shared<std::vector<SocketAddressPtr>>();};
    struct Reset{~Reset(){NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};Greeting::services={};}} reset;
    for(const auto& raw:array(request.get("clients"))->elements()){
        auto config=object(raw);auto label=config->get("label").asString(),where=config->get("map").asString();bool connected=flag(*config,"channel");Client value;
        if(connected)value=std::make_shared<NIOClient>(flag(*config,"incoming"),string(config->get("host")),int32_t(number(*config,"port",9001)),nullptr,std::make_shared<NativeNIOSelectionKey>([]{}));
        else value=std::make_shared<NIOClient>(string(config->get("host")),int32_t(number(*config,"port",9001)));
        NIOClientFixture::uid(*value,label);value->setValidGreeting(flag(*config,"valid"));value->setSentGreeting(flag(*config,"sent"));value->setConnectAttempts(int32_t(number(*config,"attempts",1)));value->setMinimaPort(int32_t(number(*config,"minimaport",-1)));if(config->containsKey("maxima"))value->setMaximaIdent(string(config->get("maxima")));
        state->clients.emplace_back(label,value);state->sync.insert(label);state->chainSync.insert(label);if(where!="none")manager->map(value,where,true);
    }
    JSONArray states;
    for(const auto& raw:array(request.get("ops"))->elements()){
        auto op=object(raw);std::string action=op->get("op").asString();state->calls=JSONArray();state->rootReads=0;JSONValue result="OK";
        try{
            if(action=="config"){
                if(op->containsKey("p2p"))GeneralParams::P2P_ENABLED=flag(*op,"p2p");if(op->containsKey("slave"))GeneralParams::TXBLOCK_NODE=flag(*op,"slave");if(op->containsKey("archive"))GeneralParams::ARCHIVE=flag(*op,"archive");if(op->containsKey("shutdown"))state->shutdown=flag(*op,"shutdown");if(op->containsKey("restore"))state->restore=flag(*op,"restore");if(op->containsKey("exists"))state->exists=flag(*op,"exists");if(op->containsKey("failure"))state->failure=op->get("failure").asString();if(op->containsKey("connectList"))GeneralParams::CONNECT_LIST=op->get("connectList").asString();if(op->containsKey("limit"))manager->RECONNECT_ATTEMPTS=int32_t(number(*op,"limit"));if(op->containsKey("invalid")){state->invalid.clear();for(const auto& host:array(op->get("invalid"))->elements())state->invalid.insert(host.asString());}
                if(op->containsKey("root"))state->root=op->get("root").isNull()?nullptr:std::make_shared<TxPoWTreeNode>(pow(*object(op->get("root"))));if(op->containsKey("last"))state->archive=op->get("last").isNull()?nullptr:std::make_shared<TxBlock>(pow(*object(op->get("last"))));if(op->containsKey("disappear"))state->disappear=flag(*op,"disappear");
            }else if(action=="set"){
                auto value=state->find(op->get("client").asString());if(op->containsKey("attempts"))value->setConnectAttempts(int32_t(number(*op,"attempts")));if(op->containsKey("valid"))value->setValidGreeting(flag(*op,"valid"));if(op->containsKey("sent"))value->setSentGreeting(flag(*op,"sent"));if(op->containsKey("maxima"))value->setMaximaIdent(string(op->get("maxima")));if(op->containsKey("host"))value->overrideHost(string(op->get("host")));if(op->containsKey("minimaport"))value->setMinimaPort(int32_t(number(*op,"minimaport")));
            }else if(action=="map")manager->map(state->find(op->get("client").asString()),op->get("map").asString(),flag(*op,"add"));
            else if(action=="disconnect")manager->disconnect(op->get("client").isNull()?std::nullopt:state->find(op->get("client").asString())->getUID(),flag(*op,"remove"));
            else if(action=="query"){
                auto method=op->get("method").asString();auto value=string(op->get("value"));Client found;if(method=="check")found=manager->checkConnected(value,flag(*op,"only"));else if(method=="host")found=manager->getNIOClient(value);else if(method=="maxima")found=manager->getMaximaUID(value);else if(method=="uid")found=manager->getNIOClientFromUID(value);else throw std::runtime_error("Unknown query");result=found?JSONValue(state->label(found)):JSONValue{};
            }else if(action=="hardShutdown")manager->hardShutDown();
            else{
                auto message=std::make_shared<Message>(string(op->get("type")));
                if(op->containsKey("client")){if(message->isMessageType(NIOManager::NIO_TXPOWREQ)||message->isMessageType(NIOManager::NIO_SYNCTXBLOCK))message->addObject("client",op->get("client").isNull()?MessageObject{}:MessageObject(op->get("client").asString()));else message->addObject("client",op->get("client").isNull()?MessageObject{}:MessageObject(state->find(op->get("client").asString())));}
                for(const char* key:{"host","uid","txpowid","reason"})if(op->containsKey(key))message->addObject(key,op->get(key).isNull()?MessageObject{}:MessageObject(op->get(key).asString()));
                if(op->containsKey("port"))message->addInteger("port",int32_t(number(*op,"port")));if(op->containsKey("reconnect"))message->addBoolean("reconnect",flag(*op,"reconnect"));
                if(message->isMessageType(NIOManager::NIO_CHECKLASTMSG)){auto now=TimerMessage::currentTimeMillis();for(const auto& [name,value]:state->clients)NIOClientFixture::last(*value,std::bit_cast<int64_t>(uint64_t(now)-uint64_t(number(*op,"age",1200000))));}
                manager->invoke(message);
            }
        }catch(const std::exception&){result="ERROR";}
        JSONObject row;row.put("result",result);row.put("state",manager->snapshot());states.add(JSONValue::parseWithException(row.toJSONString()));
    }
    return states;
}
int main(){GeneralParams::USE_SQL_COINDB=false;GeneralParams::IBDSYNC_LOGS=false;GeneralParams::NETWORKING_LOGS=false;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line))).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
