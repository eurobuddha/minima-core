#pragma once
#include "PeerRuntimeFixture.hpp"
#include "org/minima/system/network/p2p/WalkLinksFuncs.hpp"
#include "org/minima/system/network/p2p/SwapLinksFunctions.hpp"
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
#include "org/minima/system/network/p2p/UtilFuncs.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
static bool flag(const JSONObject& o,const std::string& k){return std::holds_alternative<bool>(o.get(k).value)&&o.get(k).asBool();}
static int64_t num(const JSONObject& o,const std::string& k,int64_t d=0){return o.containsKey(k)?std::get<int64_t>(o.get(k).value):d;}
namespace minima::p2p {
class P2PFunctionsFixture {public:
    static void cache(std::shared_ptr<JavaStringSet> value){std::lock_guard lock(P2PFunctions::mLocalLock);P2PFunctions::mLocalAddresses=std::move(value);}
    static void seed(int64_t seed){UtilFuncs::rnd.setSeed(uint64_t(seed));WalkLinksFuncs::random.setSeed(uint64_t(seed));}
    static int32_t draw(int32_t bound){return UtilFuncs::rnd.nextInt(bound);}
};
}
static JSONValue info(const ClientInfoPtr& value){if(!value)return {};JSONObject row;row.put("uid",nullable(value->getUID()));row.put("host",nullable(value->getHost()));row.put("port",value->getPort());row.put("incoming",value->isIncoming());row.put("connected",value->isConnected());return row;}
static JSONArray infos(const ClientInfoList& values){JSONArray rows;for(const auto& value:values)rows.add(info(value));return rows;}
static JSONArray strings(const std::shared_ptr<JavaStringSet>& values){if(!values)throw std::runtime_error("Null set");auto keys=values->elements();std::sort(keys.begin(),keys.end(),[](const auto& a,const auto& b){if(!a||!b)return !a&&bool(b);return unicode::decodeUTF8(*a)<unicode::decodeUTF8(*b);});JSONArray out;for(const auto& key:keys)out.add(nullable(key));return out;}
static PeerMapPtr map(const JSONValue& raw){if(raw.isNull())return nullptr;auto result=std::make_shared<PeerMap>();for(const auto& item:array(raw)->elements()){auto row=object(item);auto key=string(row->get("uid"));auto value=address(row->get("address"));if(key)result->put(*key,value);else result->put(nullptr,value);}return result;}
static JSONValue peerMapSnapshot(const PeerMapPtr& map){if(!map)return {};JSONArray rows;for(const auto& uid:map->nullableKeys()){JSONObject row;row.put("uid",nullable(uid));auto address=uid?map->get(*uid):map->get(nullptr);row.put("address",snapshot(address.value()));rows.add(row);}return rows;}
static JSONValue swapState(const std::shared_ptr<P2PState>& state){if(!state)return {};JSONObject row;row.put("in",peerMapSnapshot(state->getInLinks()));row.put("out",peerMapSnapshot(state->getOutLinks()));row.put("not",peerMapSnapshot(state->getNotAcceptingConnP2PLinks()));row.put("none",peerMapSnapshot(state->getNoneP2PLinks()));row.put("my",snapshot(state->getMyMinimaAddress()));row.put("hostSet",state->isHostSet());row.put("discovery",state->isDoingDiscoveryConnection());row.put("secret",state->getIpReqSecret()?JSONValue(state->getIpReqSecret()->to0xString()):JSONValue{});row.put("known",snapshots(state->getKnownPeersCopy()));return row;}
static JSONArray swapMessages(const SwapLinksFunctions::Messages& messages){JSONArray out;for(const auto& message:messages){JSONObject row;row.put("type",nullable(message->getMessageType()));row.put("uid",nullable(message->getString("uid")));if(message->exists("json"))row.put("json",*message->getObject("json").as<std::shared_ptr<JSONObject>>());out.add(row);}return out;}
static ClientInfoPtr walkClient(const JSONValue& raw){if(raw.isNull())return nullptr;auto row=object(raw);return std::make_shared<NIOClientInfo>(string(row->get("uid")),string(row->get("host")),int32_t(num(*row,"port",9001)),flag(*row,"incoming"));}
static JSONValue walkMessage(const std::shared_ptr<Message>& message){if(!message)return {};return swapMessages({message}).elements().at(0);}
static JSONValue walkSnapshot(const std::shared_ptr<P2PWalkLinks>& walk){if(!walk)return {};JSONObject row;row.put("path",snapshots(walk->getPathTaken()));row.put("secret",walk->getSecret()?JSONValue(walk->getSecret()->to0xString()):JSONValue{});row.put("returning",walk->isReturning());row.put("client",walk->isClientWalk());row.put("slots",walk->getAvailableNoneP2PConnectionSlots());row.put("joining",walk->isJoiningWalk());row.put("in",walk->isWalkInLinks());row.put("uid",nullable(walk->getTargetUidForNextHop()));return row;}
