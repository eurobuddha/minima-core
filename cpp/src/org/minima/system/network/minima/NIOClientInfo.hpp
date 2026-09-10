#pragma once
#include "NIOClient.hpp"
namespace minima {
class NIOClientInfo {
    std::shared_ptr<NIOClient> mNIOClient;
    bool mConnected=false,mIncoming=false,mValidGreeting=false;
    std::optional<std::string> mWelcome=std::string("no welcome set.."),mUID,mHost;
    int32_t mPort=0,mMinimaPort=0;
    const std::shared_ptr<NIOClient>& client() const {if(!mNIOClient)throw std::runtime_error("Null underlying client");return mNIOClient;}
public:
    NIOClientInfo(std::optional<std::string> uid,std::optional<std::string> host,int32_t port,bool incoming):mConnected(true),mIncoming(incoming),mUID(std::move(uid)),mHost(std::move(host)),mPort(port){}
    NIOClientInfo(std::shared_ptr<NIOClient> client,bool connected);
    std::optional<std::string> getUID() const{return mUID;}
    bool isIncoming() const{return mIncoming;}
    bool isConnected() const{return mConnected;}
    std::optional<std::string> getHost() const{return mHost;}
    int32_t getPort() const{return mPort;}
    int32_t getMinimaPort() const{return mMinimaPort;}
    int64_t getTimeConnected() const{return client()->getTimeConnected();}
    MessageObject getExtraData() const{return client()->getExtraData();}
    void setExtrasData(MessageObject data){client()->setExtraData(std::move(data));}
    json::JSONObject toJSON() const;
    bool ismValidGreeting() const{return mValidGreeting;}
};
}
