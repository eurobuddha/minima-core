#pragma once
#include "InetSocketAddressIO.hpp"
#include "org/minima/objects/base/MiniData.hpp"
namespace minima::p2p {
class P2PWalkLinks {
    std::optional<std::string> mTargetUidForNextHop;
    bool mWalkInLinks=false,mJoiningWalk=false,mClientWalk=false;
    int32_t mAvailableNoneP2PConnectionSlots=0;
    bool mReturning=false;
    SocketAddressList mPathTaken=std::make_shared<std::vector<SocketAddressPtr>>();
    std::shared_ptr<MiniData> mSecret=std::make_shared<MiniData>(MiniData::getRandomData(8));
public:
    P2PWalkLinks()=default;
    P2PWalkLinks(bool walkInLinks,bool joining,std::optional<std::string> target);
    P2PWalkLinks(bool walkInLinks,bool returning,SocketAddressList path);
    static P2PWalkLinks readFromJSON(const json::JSONObject& object);
    void fromJson(const json::JSONObject& object);
    json::JSONObject toJson() const;
    void addHopToPath(SocketAddressPtr address);
    SocketAddressPtr getPreviousNode(const SocketAddressPtr& address) const;
    const auto& getTargetUidForNextHop() const {return mTargetUidForNextHop;}
    void setTargetUidForNextHop(std::optional<std::string> value) {mTargetUidForNextHop=std::move(value);}
    const auto& getSecret() const {return mSecret;}
    void setSecret(std::shared_ptr<MiniData> value) {mSecret=std::move(value);}
    bool isWalkInLinks() const {return mWalkInLinks;}
    void setWalkInLinks(bool value) {mWalkInLinks=value;}
    bool isJoiningWalk() const {return mJoiningWalk;}
    void setJoiningWalk(bool value) {mJoiningWalk=value;}
    bool isClientWalk() const {return mClientWalk;}
    void setClientWalk(bool value) {mClientWalk=value;}
    int32_t getAvailableNoneP2PConnectionSlots() const {return mAvailableNoneP2PConnectionSlots;}
    void setAvailableNoneP2PConnectionSlots(int32_t value) {mAvailableNoneP2PConnectionSlots=value;}
    bool isReturning() const {return mReturning;}
    void setReturning(bool value) {mReturning=value;}
    const auto& getPathTaken() const {return mPathTaken;}
    void setPathTaken(SocketAddressList value) {mPathTaken=std::move(value);}
};
}
