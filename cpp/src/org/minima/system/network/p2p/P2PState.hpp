#pragma once
#include "support/SocketAddressSet.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/system/network/p2p/messages/InetSocketAddressIO.hpp"
#include "org/minima/system/network/p2p/params/P2PParams.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima::p2p {
using PeerMap=JavaHashMap<SocketAddressPtr>;
using PeerMapPtr=std::shared_ptr<PeerMap>;
class P2PState {
    PeerMapPtr mInLinks=std::make_shared<PeerMap>();
    PeerMapPtr mOutLinks=std::make_shared<PeerMap>();
    PeerMapPtr mNotAcceptingConnP2PLinks=std::make_shared<PeerMap>();
    PeerMapPtr mNoneP2PLinks=std::make_shared<PeerMap>();
    PeerMapPtr mAllLinks=std::make_shared<PeerMap>();
    bool mAcceptingInLinks=true;
    int32_t mMaxNumNoneP2PConnections=P2PParams::TGT_NUM_NONE_P2P_LINKS;
    int32_t mMaxNumP2PConnections=P2PParams::TGT_NUM_LINKS;
    bool mDoingDiscoveryConnection=false;
    bool mStartupComplete=false;
    int64_t mLoopDelay=P2PParams::LOOP_DELAY;
    bool mNoConnect=false;
    bool mHostSet=false;
    SocketAddressPtr mMyMinimaAddress=std::make_shared<InetSocketAddress>(9001);
    std::shared_ptr<MiniData> mIpReqSecret=std::make_shared<MiniData>();
    std::shared_ptr<SocketAddressSet> mKnownPeers=std::make_shared<SocketAddressSet>();
    mutable std::mutex mPeersMutex;
    // Protect scalar fields and reference replacement, as in NIOClient metadata.
    // Collection contents retain their existing independent synchronization.
    mutable std::mutex mStateMutex;
public:
    P2PState()=default;
    bool isAcceptingInLinks() const {std::lock_guard lock(mStateMutex);return mAcceptingInLinks;}
    void setAcceptingInLinks(bool value) {std::lock_guard lock(mStateMutex);mAcceptingInLinks=std::move(value);}
    int32_t getMaxNumNoneP2PConnections() const {std::lock_guard lock(mStateMutex);return mMaxNumNoneP2PConnections;}
    void setMaxNumNoneP2PConnections(int32_t value) {std::lock_guard lock(mStateMutex);mMaxNumNoneP2PConnections=std::move(value);}
    int32_t getMaxNumP2PConnections() const {std::lock_guard lock(mStateMutex);return mMaxNumP2PConnections;}
    void setMaxNumP2PConnections(int32_t value) {std::lock_guard lock(mStateMutex);mMaxNumP2PConnections=std::move(value);}
    bool isDoingDiscoveryConnection() const {std::lock_guard lock(mStateMutex);return mDoingDiscoveryConnection;}
    void setDoingDiscoveryConnection(bool value) {std::lock_guard lock(mStateMutex);mDoingDiscoveryConnection=std::move(value);}
    bool isStartupComplete() const {std::lock_guard lock(mStateMutex);return mStartupComplete;}
    void setStartupComplete(bool value) {std::lock_guard lock(mStateMutex);mStartupComplete=std::move(value);}
    int64_t getLoopDelay() const {std::lock_guard lock(mStateMutex);return mLoopDelay;}
    void setLoopDelay(int64_t value) {std::lock_guard lock(mStateMutex);mLoopDelay=std::move(value);}
    bool isNoConnect() const {std::lock_guard lock(mStateMutex);return mNoConnect;}
    void setNoConnect(bool value) {std::lock_guard lock(mStateMutex);mNoConnect=std::move(value);}
    bool isHostSet() const {std::lock_guard lock(mStateMutex);return mHostSet;}
    void setHostSet(bool value) {std::lock_guard lock(mStateMutex);mHostSet=std::move(value);}
    SocketAddressPtr getMyMinimaAddress() const {std::lock_guard lock(mStateMutex);return mMyMinimaAddress;}
    void setMyMinimaAddress(SocketAddressPtr value) {std::lock_guard lock(mStateMutex);mMyMinimaAddress=std::move(value);}
    std::shared_ptr<MiniData> getIpReqSecret() const {std::lock_guard lock(mStateMutex);return mIpReqSecret;}
    void setIpReqSecret(std::shared_ptr<MiniData> value) {std::lock_guard lock(mStateMutex);mIpReqSecret=std::move(value);}
    PeerMapPtr getInLinks() const {std::lock_guard lock(mStateMutex);return mInLinks;}
    void setInLinks(PeerMapPtr value) {std::lock_guard lock(mStateMutex);mInLinks=std::move(value);}
    PeerMapPtr getOutLinks() const {std::lock_guard lock(mStateMutex);return mOutLinks;}
    void setOutLinks(PeerMapPtr value) {std::lock_guard lock(mStateMutex);mOutLinks=std::move(value);}
    PeerMapPtr getNotAcceptingConnP2PLinks() const {std::lock_guard lock(mStateMutex);return mNotAcceptingConnP2PLinks;}
    void setNotAcceptingConnP2PLinks(PeerMapPtr value) {std::lock_guard lock(mStateMutex);mNotAcceptingConnP2PLinks=std::move(value);}
    PeerMapPtr getNoneP2PLinks() const {std::lock_guard lock(mStateMutex);return mNoneP2PLinks;}
    void setNoneP2PLinks(PeerMapPtr value) {std::lock_guard lock(mStateMutex);mNoneP2PLinks=std::move(value);}
    PeerMapPtr getAllLinks() const {std::lock_guard lock(mStateMutex);return mAllLinks;}
    void setAllLinks(PeerMapPtr value) {std::lock_guard lock(mStateMutex);mAllLinks=std::move(value);}
    void setMyMinimaAddress(const std::optional<std::string>& host) {auto address=std::make_shared<InetSocketAddress>(host,GeneralParams::MINIMA_PORT);setMyMinimaAddress(std::move(address));}
    void setLoopDelayToParamValue() {std::lock_guard lock(mStateMutex);mLoopDelay=P2PParams::LOOP_DELAY;}
    std::shared_ptr<SocketAddressSet> getKnownPeers() const {std::lock_guard lock(mPeersMutex);return mKnownPeers;}
    SocketAddressList getKnownPeersCopy() const {std::lock_guard lock(mPeersMutex);return std::make_shared<std::vector<SocketAddressPtr>>(mKnownPeers->elements());}
    json::JSONObject toJson() const;
};
}
