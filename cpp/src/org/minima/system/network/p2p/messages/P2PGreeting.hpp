#pragma once
#include "InetSocketAddressIO.hpp"
namespace minima::p2p {
class P2PState;
class P2PGreeting {
    int32_t mMyMinimaPort=0;
    bool mAcceptingInLinks=false;
    SocketAddressList mOutLinks=std::make_shared<std::vector<SocketAddressPtr>>();
    SocketAddressList mInLinks=std::make_shared<std::vector<SocketAddressPtr>>();
    int32_t mNumNoneP2PConnections=0;
    int32_t mMaxNumNoneP2PConnections=0;
    SocketAddressList mKnownPeers=std::make_shared<std::vector<SocketAddressPtr>>();
public:
    P2PGreeting()=default;
    explicit P2PGreeting(const P2PState& state);
    P2PGreeting(int32_t port,SocketAddressList knownPeers);
    P2PGreeting(int32_t port,bool accepting,SocketAddressList outLinks,SocketAddressList inLinks,int32_t count,int32_t maximum,SocketAddressList knownPeers);
    static P2PGreeting fromJSON(const json::JSONObject& object);
    json::JSONObject toJson() const;
    bool equals(const P2PGreeting* other) const;
    int32_t getMyMinimaPort() const {return mMyMinimaPort;}
    void setMyMinimaPort(int32_t value) {mMyMinimaPort=std::move(value);}
    bool isAcceptingInLinks() const {return mAcceptingInLinks;}
    void setAcceptingInLinks(bool value) {mAcceptingInLinks=std::move(value);}
    SocketAddressList getOutLinks() const {return mOutLinks;}
    void setOutLinks(SocketAddressList value) {mOutLinks=std::move(value);}
    SocketAddressList getInLinks() const {return mInLinks;}
    void setInLinks(SocketAddressList value) {mInLinks=std::move(value);}
    int32_t getNumNoneP2PConnections() const {return mNumNoneP2PConnections;}
    void setNumNoneP2PConnections(int32_t value) {mNumNoneP2PConnections=std::move(value);}
    int32_t getMaxNumNoneP2PConnections() const {return mMaxNumNoneP2PConnections;}
    void setMaxNumNoneP2PConnections(int32_t value) {mMaxNumNoneP2PConnections=std::move(value);}
    SocketAddressList getKnownPeers() const {return mKnownPeers;}
    void setKnownPeers(SocketAddressList value) {mKnownPeers=std::move(value);}
};
}
