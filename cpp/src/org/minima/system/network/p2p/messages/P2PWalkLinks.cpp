#include "P2PWalkLinks.hpp"
namespace minima::p2p {
P2PWalkLinks::P2PWalkLinks(bool walkInLinks,bool joining,std::optional<std::string> target) {mWalkInLinks=walkInLinks;mJoiningWalk=joining;mTargetUidForNextHop=std::move(target);}
P2PWalkLinks::P2PWalkLinks(bool walkInLinks,bool returning,SocketAddressList path) {mWalkInLinks=walkInLinks;mReturning=returning;mPathTaken=std::move(path);}
P2PWalkLinks P2PWalkLinks::readFromJSON(const json::JSONObject& object) {P2PWalkLinks result;result.fromJson(object);return result;}
void P2PWalkLinks::addHopToPath(SocketAddressPtr address) {if(!address)throw std::runtime_error("Address is null");if(!mPathTaken)throw std::runtime_error("Null path");mPathTaken->push_back(std::move(address));}
SocketAddressPtr P2PWalkLinks::getPreviousNode(const SocketAddressPtr& address) const {
    if(!mPathTaken)throw std::runtime_error("Null path");
    for(size_t i=0;i<mPathTaken->size();++i){const auto& next=mPathTaken->at(i);if((!next&&!address)||(next&&address&&address->equals(*next)))return i?mPathTaken->at(i-1):nullptr;}return nullptr;
}
void P2PWalkLinks::fromJson(const json::JSONObject& object) {
    auto contents=std::get<std::shared_ptr<json::JSONObject>>(object.get("walk_links").value);if(!contents)throw std::runtime_error("Null walk contents");
    if(contents->containsKey("secret"))setSecret(std::make_shared<MiniData>(MiniData::fromHex(contents->get("secret").asString())));
    if(contents->containsKey("walkInLinks"))setWalkInLinks(contents->get("walkInLinks").asBool());
    if(contents->containsKey("isJoiningWalk"))setJoiningWalk(contents->get("isJoiningWalk").asBool());
    if(contents->containsKey("isClientWalk"))setClientWalk(contents->get("isClientWalk").asBool());
    if(contents->containsKey("availableClientSlots"))setAvailableNoneP2PConnectionSlots(InetSocketAddressIO::safeReadInt(*contents,"availableClientSlots"));
    if(contents->containsKey("isReturning"))setReturning(contents->get("isReturning").asBool());
    if(contents->containsKey("pathTaken"))setPathTaken(InetSocketAddressIO::addressesJSONToList(std::get<std::shared_ptr<json::JSONArray>>(contents->get("pathTaken").value)));
}
json::JSONObject P2PWalkLinks::toJson() const {
    if(!mSecret)throw std::runtime_error("Null walk secret");json::JSONObject contents,result;
    contents.put("secret",mSecret->to0xString());contents.put("walkInLinks",mWalkInLinks);contents.put("isJoiningWalk",mJoiningWalk);contents.put("isClientWalk",mClientWalk);contents.put("availableClientSlots",mAvailableNoneP2PConnectionSlots);contents.put("isReturning",mReturning);contents.put("pathTaken",InetSocketAddressIO::addressesListToJSON(mPathTaken));result.put("walk_links",contents);return result;
}
}
