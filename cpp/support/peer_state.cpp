#include "PeerHarness.hpp"
#include "org/minima/system/network/p2p/P2PDB.hpp"
#include "org/minima/system/network/p2p/P2PState.hpp"
#include "org/minima/system/network/p2p/messages/P2PGreeting.hpp"
#include "org/minima/system/network/p2p/params/P2PTestParams.hpp"
#include <map>
static PeerMapPtr map(const P2PState& state,const std::string& name){
if(name=="InLinks")return state.getInLinks();
if(name=="OutLinks")return state.getOutLinks();
if(name=="NotAcceptingConnP2PLinks")return state.getNotAcceptingConnP2PLinks();
if(name=="NoneP2PLinks")return state.getNoneP2PLinks();
if(name=="AllLinks")return state.getAllLinks();
throw std::invalid_argument(name);}
static void setMap(P2PState& state,const std::string& name,PeerMapPtr values){
if(name=="InLinks"){state.setInLinks(values);return;}
if(name=="OutLinks"){state.setOutLinks(values);return;}
if(name=="NotAcceptingConnP2PLinks"){state.setNotAcceptingConnP2PLinks(values);return;}
if(name=="NoneP2PLinks"){state.setNoneP2PLinks(values);return;}
if(name=="AllLinks"){state.setAllLinks(values);return;}
throw std::invalid_argument(name);}
static JSONValue mapSnapshot(const PeerMapPtr& map){if(!map)return {};JSONArray out;auto keys=map->nullableKeys();auto values=map->elements();for(size_t i=0;i<keys.size();++i){JSONObject item;item.put("key",nullable(keys[i]));item.put("address",snapshot(values[i]));out.add(item);}return out;}
static JSONObject stateSnapshot(const P2PState& state){JSONObject out;
out.put("AcceptingInLinks",state.isAcceptingInLinks());
out.put("MaxNumNoneP2PConnections",state.getMaxNumNoneP2PConnections());
out.put("MaxNumP2PConnections",state.getMaxNumP2PConnections());
out.put("DoingDiscoveryConnection",state.isDoingDiscoveryConnection());
out.put("StartupComplete",state.isStartupComplete());
out.put("LoopDelay",state.getLoopDelay());
out.put("NoConnect",state.isNoConnect());
out.put("HostSet",state.isHostSet());
out.put("InLinks",mapSnapshot(state.getInLinks()));
out.put("OutLinks",mapSnapshot(state.getOutLinks()));
out.put("NotAcceptingConnP2PLinks",mapSnapshot(state.getNotAcceptingConnP2PLinks()));
out.put("NoneP2PLinks",mapSnapshot(state.getNoneP2PLinks()));
out.put("AllLinks",mapSnapshot(state.getAllLinks()));
out.put("address",snapshot(state.getMyMinimaAddress()));out.put("secret",state.getIpReqSecret()?JSONValue(state.getIpReqSecret()->to0xString()):JSONValue{});out.put("known",snapshots(state.getKnownPeersCopy()));try{out.put("json",state.toJson());}catch(const std::exception&){out.put("json","ERROR");}return out;}
static JSONObject greetingSnapshot(const P2PGreeting& greeting){JSONObject out;
out.put("MyMinimaPort",greeting.getMyMinimaPort());
out.put("AcceptingInLinks",greeting.isAcceptingInLinks());
out.put("NumNoneP2PConnections",greeting.getNumNoneP2PConnections());
out.put("MaxNumNoneP2PConnections",greeting.getMaxNumNoneP2PConnections());
out.put("KnownPeers",snapshots(greeting.getKnownPeers()));
out.put("InLinks",snapshots(greeting.getInLinks()));
out.put("OutLinks",snapshots(greeting.getOutLinks()));
try{out.put("json",greeting.toJson());}catch(const std::exception&){out.put("json","ERROR");}return out;}
static PeerMapPtr checked(PeerMapPtr map){if(!map)throw std::runtime_error("Null peer map");return map;}
static JSONValue put(PeerMapPtr map,const JSONValue& key,const JSONValue& value){checked(map);auto old=key.isNull()?map->put(nullptr,address(value)):map->put(key.asString(),address(value));return snapshot(old.value_or(nullptr));}
static int32_t integer(const JSONValue& value){return std::bit_cast<int32_t>(uint32_t(std::get<int64_t>(value.value)));}
static JSONObject parameterSnapshot(){JSONObject out;out.put("VERSION",P2PParams::VERSION);out.put("LOG_LEVEL",!P2PParams::LOG_LEVEL?JSONValue{}:P2PParams::LOG_LEVEL==P2PLogLevel::NODE_RUNNER_MSG?"NODE_RUNNER_MSG":P2PParams::LOG_LEVEL==P2PLogLevel::INFO?"INFO":"DEBUG");
out.put("PEERS_LIST_SIZE",P2PParams::PEERS_LIST_SIZE);
out.put("TGT_NUM_LINKS",P2PParams::TGT_NUM_LINKS);
out.put("TGT_NUM_NONE_P2P_LINKS",P2PParams::TGT_NUM_NONE_P2P_LINKS);
out.put("MIN_NUM_CONNECTIONS",P2PParams::MIN_NUM_CONNECTIONS);
out.put("LOOP_DELAY",P2PParams::LOOP_DELAY);
out.put("HASH_RATE_UPDATE_DELAY",P2PParams::HASH_RATE_UPDATE_DELAY);
out.put("LOOP_DELAY_VARIABILITY",P2PParams::LOOP_DELAY_VARIABILITY);
out.put("NODE_NOT_ACCEPTING_CHECK_DELAY",P2PParams::NODE_NOT_ACCEPTING_CHECK_DELAY);
out.put("SAVE_DATA_DELAY",P2PParams::SAVE_DATA_DELAY);
out.put("WALK_LINKS_EXPIRE_TIME",P2PParams::WALK_LINKS_EXPIRE_TIME);
out.put("AUTH_KEY_EXPIRY",P2PParams::AUTH_KEY_EXPIRY);
out.put("METRICS_DELAY",P2PParams::METRICS_DELAY);
SocketAddressList nodes;if(P2PParams::DEFAULT_NODE_LIST)nodes=std::make_shared<std::vector<SocketAddressPtr>>(P2PParams::DEFAULT_NODE_LIST->begin(),P2PParams::DEFAULT_NODE_LIST->end());out.put("nodes",snapshots(nodes));return out;}
static const std::pair<std::string,int32_t P2PTestParams::*> testIntegers[]={
{"TGT_NUM_LINKS",&P2PTestParams::TGT_NUM_LINKS},
{"TGT_NUM_NONE_P2P_LINKS",&P2PTestParams::TGT_NUM_NONE_P2P_LINKS},
{"MIN_NUM_CONNECTIONS",&P2PTestParams::MIN_NUM_CONNECTIONS},
{"LOOP_DELAY",&P2PTestParams::LOOP_DELAY},
{"HASH_RATE_UPDATE_DELAY",&P2PTestParams::HASH_RATE_UPDATE_DELAY},
{"LOOP_DELAY_VARIABILITY",&P2PTestParams::LOOP_DELAY_VARIABILITY},
{"NODE_NOT_ACCEPTING_CHECK_DELAY",&P2PTestParams::NODE_NOT_ACCEPTING_CHECK_DELAY},
{"SAVE_DATA_DELAY",&P2PTestParams::SAVE_DATA_DELAY},
{"WALK_LINKS_EXPIRE_TIME",&P2PTestParams::WALK_LINKS_EXPIRE_TIME},
{"AUTH_KEY_EXPIRY",&P2PTestParams::AUTH_KEY_EXPIRY},
{"METRICS_DELAY",&P2PTestParams::METRICS_DELAY},
};
static JSONValue testNodes(const std::shared_ptr<JavaList<SocketAddressPtr>>& nodes){return nodes?snapshots(std::make_shared<std::vector<SocketAddressPtr>>(nodes->begin(),nodes->end())):JSONValue{};}
static JSONObject testSettingsSnapshot(){const auto& settings=P2PTestParams::values();JSONObject out;for(const auto& [name,member]:testIntegers)out.put(name,settings.*member);out.put("LOG_LEVEL",!settings.LOG_LEVEL?JSONValue{}:settings.LOG_LEVEL==P2PLogLevel::NODE_RUNNER_MSG?"NODE_RUNNER_MSG":settings.LOG_LEVEL==P2PLogLevel::INFO?"INFO":"DEBUG");out.put("METRICS_URL",nullable(settings.METRICS_URL));out.put("nodes",testNodes(settings.DEFAULT_NODE_LIST));return out;}
static JSONValue runTestSettings(const JSONObject& request){
    auto& settings=P2PTestParams::values();JSONArray output;
    auto capture=[&](JSONValue result){JSONObject row;row.put("result",result);row.put("test",testSettingsSnapshot());try{row.put("active",parameterSnapshot());}catch(const std::exception&){row.put("active","ERROR");}row.put("alias",settings.DEFAULT_NODE_LIST==P2PParams::DEFAULT_NODE_LIST);output.add(row);};
    capture("INITIAL");
    for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);JSONValue result="OK";try{auto name=op->get("op").asString();auto value=op->get("value");
        if(name=="apply")P2PTestParams::setTestParams();
        else if(name=="set"){auto field=op->get("field").asString();if(field=="LOG_LEVEL")settings.LOG_LEVEL=value.isNull()?std::nullopt:std::optional<P2PLogLevel>(P2PLogLevel(integer(value)));else if(field=="METRICS_URL")settings.METRICS_URL=string(value);else{bool found=false;for(const auto& [key,member]:testIntegers)if(field==key){settings.*member=integer(value);found=true;break;}if(!found)throw std::invalid_argument(field);}}
        else if(name=="replace"){auto nodes=list(value);settings.DEFAULT_NODE_LIST=nodes?std::make_shared<JavaList<SocketAddressPtr>>(*nodes,false):nullptr;}
        else {auto nodes=op->get("active").isNull()||!op->get("active").asBool()?settings.DEFAULT_NODE_LIST:P2PParams::DEFAULT_NODE_LIST;if(!nodes)throw std::runtime_error("Null list");if(name=="append")nodes->add(address(value));else if(name=="clear")nodes->clear();else if(name=="element")result=snapshot(nodes->set(0,address(value)));else throw std::invalid_argument(name);}
    }catch(const std::exception&){result="ERROR";}capture(result);}return output;
}
static JSONValue runState(JSONObject& request){GeneralParams::MINIMA_PORT=9001;GeneralParams::IS_MOBILE=false;P2PParams::LOOP_DELAY=600000;P2PParams::TGT_NUM_LINKS=5;P2PParams::TGT_NUM_NONE_P2P_LINKS=100;
    auto kind=request.get("kind").asString();
    if(kind=="testParams")return runTestSettings(request);
    if(kind=="state"){P2PState state;std::map<std::string,PeerMapPtr> external;SocketAddressList copy;JSONArray out;out.add(stateSnapshot(state));for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto name=op->get("op").asString(),field=op->containsKey("field")?op->get("field").asString():"";const auto& value=op->get("value");JSONValue result="OK";try{
        if(name=="set"){
if(field=="AcceptingInLinks")state.setAcceptingInLinks(value.asBool());
else if(field=="MaxNumNoneP2PConnections")state.setMaxNumNoneP2PConnections(integer(value));
else if(field=="MaxNumP2PConnections")state.setMaxNumP2PConnections(integer(value));
else if(field=="DoingDiscoveryConnection")state.setDoingDiscoveryConnection(value.asBool());
else if(field=="StartupComplete")state.setStartupComplete(value.asBool());
else if(field=="LoopDelay")state.setLoopDelay(std::get<int64_t>(value.value));
else if(field=="NoConnect")state.setNoConnect(value.asBool());
else if(field=="HostSet")state.setHostSet(value.asBool());
else throw std::invalid_argument(field);}
        else if(name=="address")state.setMyMinimaAddress(address(value));
        else if(name=="host")state.setMyMinimaAddress(string(value));
        else if(name=="secret")state.setIpReqSecret(value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString())));
        else if(name=="port")GeneralParams::MINIMA_PORT=integer(value);
        else if(name=="mobile")GeneralParams::IS_MOBILE=value.asBool();
        else if(name=="loopParam")P2PParams::LOOP_DELAY=integer(value);
        else if(name=="loopReset")state.setLoopDelayToParamValue();
        else if(name=="put")result=put(map(state,field),op->get("key"),value);
        else if(name=="remove"){auto m=checked(map(state,field));auto old=op->get("key").isNull()?m->remove(nullptr):m->remove(op->get("key").asString());result=snapshot(old.value_or(nullptr));}
        else if(name=="clear")checked(map(state,field))->clear();
        else if(name=="replace"){PeerMapPtr m=value.isNull()?nullptr:std::make_shared<PeerMap>();if(m)for(const auto& rawEntry:array(value)->elements()){auto entry=object(rawEntry);put(m,entry->get("key"),entry->get("value"));}external[field]=m;setMap(state,field,m);}
        else if(name=="external")result=put(external[field],op->get("key"),value);
        else if(name=="alias")setMap(state,field,map(state,value.asString()));
        else if(name=="knownAdd")result=state.getKnownPeers()->add(address(value));
        else if(name=="knownRemove")result=state.getKnownPeers()->remove(address(value));
        else if(name=="knownContains")result=state.getKnownPeers()->contains(address(value));
        else if(name=="knownClear")state.getKnownPeers()->clear();
        else if(name=="knownCopy")copy=state.getKnownPeersCopy();
        else if(name=="copyAppend"){if(!copy)throw std::runtime_error("Null copy");copy->push_back(address(value));}
        else if(name=="greeting")result=greetingSnapshot(P2PGreeting(state));
        else throw std::invalid_argument(name);
        }catch(const std::exception&){result="ERROR";}JSONObject snap;snap.put("result",result);snap.put("state",stateSnapshot(state));snap.put("copy",snapshots(copy));out.add(snap);}return out;}
    if(kind=="greeting"){auto known=list(request.get("known")),in=list(request.get("in")),outLinks=list(request.get("out"));auto ctor=request.get("ctor").asString();P2PGreeting greeting=ctor=="short"?P2PGreeting(9121,known):ctor=="full"?P2PGreeting(9121,true,outLinks,in,5,20,known):P2PGreeting();JSONArray out;out.add(greetingSnapshot(greeting));for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto name=op->get("op").asString(),field=op->containsKey("field")?op->get("field").asString():"";const auto& value=op->get("value");JSONValue result="OK";try{
        if(name=="set"){
if(field=="MyMinimaPort")greeting.setMyMinimaPort(integer(value));
else if(field=="AcceptingInLinks")greeting.setAcceptingInLinks(value.asBool());
else if(field=="NumNoneP2PConnections")greeting.setNumNoneP2PConnections(integer(value));
else if(field=="MaxNumNoneP2PConnections")greeting.setMaxNumNoneP2PConnections(integer(value));
else if(field=="KnownPeers")greeting.setKnownPeers(list(value));
else if(field=="InLinks")greeting.setInLinks(list(value));
else if(field=="OutLinks")greeting.setOutLinks(list(value));
else throw std::invalid_argument(field);}
        else if(name=="append"||name=="external"){SocketAddressList target;
if(field=="KnownPeers")target=name=="append"?greeting.getKnownPeers():known;
else if(field=="InLinks")target=name=="append"?greeting.getInLinks():in;
else if(field=="OutLinks")target=name=="append"?greeting.getOutLinks():outLinks;
else throw std::invalid_argument(field);if(!target)throw std::runtime_error("Null list");target->push_back(address(value));}
        else if(name=="read")greeting=P2PGreeting::fromJSON(*object(value));
        else if(name=="equal"){auto other=P2PGreeting::fromJSON(*object(value));result=greeting.equals(&other);}
        else if(name=="self")result=greeting.equals(&greeting);
        else if(name=="null")result=greeting.equals(nullptr);
        else if(name=="roundtrip"){auto json=greeting.toJson();auto other=P2PGreeting::fromJSON(*object(json.get("greeting")));result=greeting.equals(&other);}
        else throw std::invalid_argument(name);
        }catch(const std::exception&){result="ERROR";}JSONObject snap;snap.put("result",result);snap.put("greeting",greetingSnapshot(greeting));out.add(snap);}return out;}
    if(kind=="params"){auto out=parameterSnapshot();JSONArray operations;try{P2PParams::DEFAULT_NODE_LIST->add(address(request.get("value")));operations.add("OK");}catch(const std::exception&){operations.add("ERROR");}try{P2PParams::DEFAULT_NODE_LIST->clear();operations.add("OK");}catch(const std::exception&){operations.add("ERROR");}try{P2PParams::DEFAULT_NODE_LIST->set(0,address(request.get("value")));operations.add("OK");}catch(const std::exception&){operations.add("ERROR");}out.put("operations",operations);return out;}
    if(kind=="db"){P2PParams::VERSION="1.1";P2PDB db;auto external=list(request.get("peers"));SocketAddressList extracted;JSONArray out;for(const auto& raw:array(request.get("ops"))->elements()){auto op=object(raw);auto name=op->get("op").asString();const auto& value=op->get("value");JSONValue result="OK";try{
    if(name=="set")db.setPeersList(list(value));
    else if(name=="externalSet")db.setPeersList(external);
    else if(name=="externalAppend"){if(!external)throw std::runtime_error("Null external");external->push_back(address(value));}
    else if(name=="get"){extracted=db.getPeersList();result=snapshots(extracted);}
    else if(name=="append"){if(!extracted)throw std::runtime_error("Null extracted");extracted->push_back(address(value));}
    else if(name=="jsonPut"){auto all=db.getAllData();if(!all)throw std::runtime_error("Null DB");all->put(op->get("key").asString(),value);}
    else if(name=="read"){auto bytes=MiniData::fromHex(value.asString());size_t offset=0;db.readDataStream(bytes.bytes(),offset);}
    else if(name=="roundtrip"){auto bytes=db.serialise();size_t offset=0;db.readDataStream(bytes,offset);}
    else if(name=="version")db.setVersion();
    else if(name=="versionParam")P2PParams::VERSION=value.asString();
    else throw std::invalid_argument(name);
    }catch(const std::exception&){result="ERROR";}JSONObject snap;snap.put("result",result);snap.put("data",db.getAllData()?JSONValue(*db.getAllData()):JSONValue{});try{snap.put("peers",snapshots(db.getPeersList()));}catch(const std::exception&){snap.put("peers","ERROR");}try{snap.put("version",db.getVersion());}catch(const std::exception&){snap.put("version","ERROR");}try{snap.put("wire",MiniData(db.serialise()).to0xString());}catch(const std::exception&){snap.put("wire","ERROR");}snap.put("extracted",snapshots(extracted));out.add(JSONValue::parseWithException(snap.toJSONString()));}return out;}
    throw std::invalid_argument(kind);
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto request=object(JSONValue::parseWithException(line));std::cout<<runState(*request).toJSONString()<<'\n';}catch(const std::exception&){std::cout<<"\"ERROR\"\n";}}
