#include "PeerRuntimeFixture.hpp"
#include "org/minima/system/network/p2p/P2PPeersChecker.hpp"
#include "org/minima/database/txpowtree/TxPowTree.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
static bool flag(const JSONObject& o,const std::string& key){return std::holds_alternative<bool>(o.get(key).value)&&o.get(key).asBool();}
static int64_t number(const JSONObject& o,const std::string& key,int64_t value=0){return o.containsKey(key)?std::get<int64_t>(o.get(key).value):value;}
static JSONObject event(const std::shared_ptr<Message>& message){JSONObject row;row.put("type",nullable(message->getMessageType()));auto raw=message->getObject("address");row.put("address",snapshot(raw.isNull()?nullptr:raw.as<SocketAddressPtr>()));for(const char* key:{"force","forcelog"})if(message->exists(key))row.put(key,message->getBoolean(key));return row;}
class Queue : public MessageStack {public:JSONArray drain(){JSONArray rows;while(auto message=getNextMessage())rows.add(event(message));return rows;}};
namespace minima::p2p {class P2PFunctionsFixture {public:static void resetLocal(){std::lock_guard lock(P2PFunctions::mLocalLock);P2PFunctions::mLocalAddresses=std::make_shared<JavaStringSet>();}};}
class Checker : public P2PPeersChecker {
    Checker(const std::shared_ptr<MessageStack>& parent,std::function<std::shared_ptr<TxPoWTreeNode>()> tip):P2PPeersChecker(parent,std::move(tip),[]{}){}
public:
    JSONArray timers;
    bool timerFailure=false;
    static std::shared_ptr<Checker> create(const std::shared_ptr<MessageStack>& parent,std::function<std::shared_ptr<TxPoWTreeNode>()> tip){auto checker=std::shared_ptr<Checker>(new Checker(parent,std::move(tip)));while(checker->getNextMessage()){}return checker;}
    void process(const std::shared_ptr<Message>& message){processMessage(message);}
    JSONArray drain(){JSONArray rows;while(auto message=getNextMessage())rows.add(event(message));return rows;}
    void PostTimerMessage(const std::shared_ptr<TimerMessage>& timer)override{if(timerFailure)throw std::runtime_error("Timer receiver failure");auto row=event(timer);row.put("delay",NIOManagerFixture::timerDelay(*timer));timers.add(row);}
};
static MessageObject boxed(const JSONValue& value){if(value.isNull())return {};if(std::holds_alternative<bool>(value.value))return value.asBool();if(std::holds_alternative<int64_t>(value.value))return std::get<int64_t>(value.value);return value.asString();}
static void append(JSONArray& to,const JSONArray& from){for(const auto& value:from.elements())to.add(value);}
static JSONValue runChecker(const JSONObject& request){
    JSONArray events,posted,parentPosted,states;auto nio=NIOManagerFixture::create();NIOManager::runtimeLookup=[nio]{return nio;};NIOClient::runtimeLookup=[nio]{return nio->clientRuntime();};
    struct Reset{~Reset(){NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};P2PFunctions::logger={};P2PFunctions::openURL={};P2PFunctionsFixture::resetLocal();}}reset;
    if(flag(request,"connected")){auto value=std::make_shared<NIOClient>(false,"8.8.8.8",9001,nullptr,std::make_shared<NativeNIOSelectionKey>([]{}));NIOClientFixture::uid(*value,"c0");nio->add(value,true);}
    if(flag(request,"missingManager")||flag(request,"noMain"))NIOManager::runtimeLookup=[]{return std::shared_ptr<NIOManager>{};};
    GeneralParams::TEST_PARAMS=flag(request,"test");GeneralParams::ALLOW_ALL_IP=flag(request,"allow");GeneralParams::NETWORKING_LOGS=false;GeneralParams::PEERSCHECKER_lOG=flag(request,"logs");P2PParams::LOG_LEVEL=P2PLogLevel(number(request,"level",2));P2PPeersChecker::MAX_VERIFIED_PEERS=int32_t(number(request,"max",250));P2PFunctionsFixture::resetLocal();
    P2PFunctions::logger=[&](const auto& line){if(!flag(request,"noMain"))events.add("log:"+line);};P2PFunctions::openURL=[&](const auto& url){events.add("available");if(flag(request,"offline"))throw wire::IOException("Offline fixture");};
    auto tree=std::make_shared<TxPowTree>();std::shared_ptr<TxPoWTreeNode> first,last;if(request.containsKey("chain"))for(const auto& item:array(request.get("chain"))->elements()){auto bytes=MiniData::fromHex(item.asString());size_t at=0;auto node=std::make_shared<TxPoWTreeNode>(TxPoW::deserialise(bytes.bytes(),at));if(last)last->addChildNode(node);else first=node;last=node;}if(first)tree->setRoot(first);
    auto parent=std::make_shared<Queue>();auto checker=Checker::create(flag(request,"missingParent")?nullptr:parent,[&]{events.add("tip");if(flag(request,"tipFailure"))throw std::runtime_error("Tip failure");return tree->getTip();});checker->timerFailure=flag(request,"timerFailure");checker->PEERS_LOOP_TIMER=number(request,"loopDelay",21600000);
    for(const char* key:{"verified","unverified"})if(request.containsKey(key)){auto peers=std::string(key)=="verified"?checker->getVerifiedPeers():checker->getUnverifiedPeers();auto values=list(request.get(key));for(const auto& value:*values)peers->add(value);}
    auto verified=checker->getVerifiedPeers(),unverified=checker->getUnverifiedPeers();
    for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);JSONValue result="OK";try{auto kind=op->get("op").asString();auto addressValue=address(op->get("address"));if(kind=="have")result=checker->haveAnyPeers();else if(kind=="admit")checker->checkUnverifiedPeer(addressValue);else if(kind=="verify")verified->add(addressValue);else if(kind=="unverify")unverified->add(addressValue);else if(kind=="clearPending")unverified->clear();else if(kind=="clear"){verified->clear();unverified->clear();}else if(kind=="identity")result=verified==checker->getVerifiedPeers()&&unverified==checker->getUnverifiedPeers();else{
        std::optional<std::string> type=kind=="add"?P2PPeersChecker::PEERS_ADDPEERS:kind=="check"?P2PPeersChecker::PEERS_CHECKPEERS:kind=="loop"?P2PPeersChecker::PEERS_LOOP:kind=="force"?P2PPeersChecker::PEERS_FORCEFULLCHECK:kind=="init"?P2PPeersChecker::PEERS_INIT:string(op->get("type"));auto message=std::make_shared<Message>(type);message->addObject("address",MessageObject(addressValue));for(const char* key:{"force","forcelog"})if(op->containsKey(key))message->addObject(key,boxed(op->get(key)));checker->process(flag(*op,"nullMessage")?nullptr:message);
    }}catch(const std::exception&){result="ERROR";}
        append(posted,checker->drain());append(parentPosted,parent->drain());JSONObject row;row.put("result",result);row.put("events",events);row.put("posted",posted);row.put("parent",parentPosted);row.put("timers",checker->timers);row.put("verified",snapshots(std::make_shared<std::vector<SocketAddressPtr>>(verified->elements())));row.put("unverified",snapshots(std::make_shared<std::vector<SocketAddressPtr>>(unverified->elements())));row.put("traffic",nio->getTrafficListener()->getBreakdown());row.put("read",nio->getTrafficListener()->getTotalRead());row.put("write",nio->getTrafficListener()->getTotalWrite());states.add(JSONValue::parseWithException(row.toJSONString()));
    }return states;
}
int main(){std::string line;while(std::getline(std::cin,line))try{std::cout<<runChecker(*object(JSONValue::parseWithException(line))).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
