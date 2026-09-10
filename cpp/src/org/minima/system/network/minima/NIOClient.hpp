#pragma once
#include "support/NIORuntime.hpp"
#include "support/NIOSocket.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/FastByteArrayStream.hpp"
#include <deque>
#include <cstring>
#include <bit>
namespace minima {
class NIOClient {
    struct Buffer {
        std::vector<uint8_t> bytes=std::vector<uint8_t>(8192);
        int32_t position=0,limit=8192;
        int32_t remaining() const {return limit-position;}
        void flip(){limit=position;position=0;}
        void compact(){auto left=remaining();std::memmove(bytes.data(),bytes.data()+position,size_t(left));position=left;limit=8192;}
    };
    std::unique_ptr<Buffer> mBufferIn,mBufferOut;
    int32_t mReadCurrentPosition=0,mReadCurrentLimit=0;
    std::vector<uint8_t> mReadByteArrayTemp;
    std::unique_ptr<FastByteArrayStream> mReadByteArray;
    int32_t mWritePosition=0,mWriteLimit=0;
    bool mWriteStart=false;
    std::shared_ptr<MiniData> mWriteData;
    std::shared_ptr<NIOSocket> mSocket;
    std::shared_ptr<NIOSelectionKey> mKey;
    std::unique_ptr<std::deque<std::shared_ptr<MiniData>>> mMessages;
    std::mutex mMessagesLock;
    std::shared_ptr<NIORuntime> mRuntime;
    mutable std::mutex mMetadataLock;
    MessageObject mExtraData;
    std::optional<std::string> mUID=std::string();
    std::optional<std::string> mHost=std::string();
    int32_t mPort=0;
    int32_t mMinimaPort=-1;
    bool mIncoming=false;
    std::optional<std::string> mWelcomeMessage=std::string();
    int64_t mTimeConnected=0;
    int64_t mLastMessageRead=0;
    int32_t mConnectAttempts=1;
    bool mValidGreeting=false;
    bool mSentGreeting=false;
    bool mP2PGreeting=false;
    std::optional<std::string> mMaximaIdent=std::string();
    std::optional<std::string> mMaximaMLS=std::string();
    bool mHasMaximaDisconnected=false;
    static std::shared_ptr<NIORuntime> requireRuntime();
    static json::JSONValue nullable(const std::optional<std::string>& value){return value?json::JSONValue(*value):json::JSONValue{};}
    friend struct NIOClientFixture;
public:
    static std::atomic<bool> mTraceON;
    static constexpr int32_t MAX_NIO_BUFFERS=8*1024,MAX_MESSAGE=256*1024*1024;
    static std::function<std::shared_ptr<NIORuntime>()> runtimeLookup;
    NIOClient(std::optional<std::string> host,int32_t port);
    NIOClient(bool incoming,std::optional<std::string> host,int32_t port,std::shared_ptr<NIOSocket> socket,std::shared_ptr<NIOSelectionKey> key);
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toString();}
    void setExtraData(MessageObject value){std::lock_guard lock(mMetadataLock);mExtraData=std::move(value);}
    MessageObject getExtraData() const{std::lock_guard lock(mMetadataLock);return mExtraData;}
    std::optional<std::string> getUID() const{std::lock_guard lock(mMetadataLock);return mUID;}
    bool isIncoming() const{std::lock_guard lock(mMetadataLock);return mIncoming;}
    bool isOutgoing() const{std::lock_guard lock(mMetadataLock);return !mIncoming;}
    bool isMaximaClient() const {std::lock_guard lock(mMetadataLock);return !mMaximaIdent.value().empty();}
    void setMaximaIdent(std::optional<std::string> value){std::lock_guard lock(mMetadataLock);mMaximaIdent=std::move(value);}
    std::optional<std::string> getMaximaIdent() const{std::lock_guard lock(mMetadataLock);return mMaximaIdent;}
    bool isMaximaMLS() const {std::lock_guard lock(mMetadataLock);return !mMaximaMLS.value().empty();}
    void setMaximaMLS(std::optional<std::string> value){std::lock_guard lock(mMetadataLock);mMaximaMLS=std::move(value);}
    std::optional<std::string> getMaximaMLS() const{std::lock_guard lock(mMetadataLock);return mMaximaMLS;}
    bool hasMaximaDiscxonnected() const{std::lock_guard lock(mMetadataLock);return mHasMaximaDisconnected;}
    void setMaximaDisconnected(){std::lock_guard lock(mMetadataLock);mHasMaximaDisconnected=true;}
    void overrideHost(std::optional<std::string> value){std::lock_guard lock(mMetadataLock);mHost=std::move(value);}
    std::optional<std::string> getHost() const{std::lock_guard lock(mMetadataLock);return mHost;}
    void setPort(int32_t value){std::lock_guard lock(mMetadataLock);mPort=value;}
    int32_t getPort() const{std::lock_guard lock(mMetadataLock);return mPort;}
    void setMinimaPort(int32_t value){std::lock_guard lock(mMetadataLock);mMinimaPort=value;}
    int32_t getMinimaPort() const{std::lock_guard lock(mMetadataLock);return mMinimaPort;}
    std::string getFullAddress() const{std::lock_guard lock(mMetadataLock);return mHost.value_or("null")+":"+std::to_string(mMinimaPort!=-1?mMinimaPort:mPort);}
    std::string getFullMinimaAddress() const{return getFullAddress();}
    bool isValidGreeting() const{std::lock_guard lock(mMetadataLock);return mValidGreeting;}
    void setValidGreeting(bool value){std::lock_guard lock(mMetadataLock);mValidGreeting=value;}
    bool haveSentGreeting() const{std::lock_guard lock(mMetadataLock);return mSentGreeting;}
    void setReceivedP2PGreeting(){std::lock_guard lock(mMetadataLock);mP2PGreeting=true;}
    bool hasReceivedP2PGreeting() const{std::lock_guard lock(mMetadataLock);return mP2PGreeting;}
    void setSentGreeting(bool value){std::lock_guard lock(mMetadataLock);mSentGreeting=value;}
    std::optional<std::string> getWelcomeMessage() const{std::lock_guard lock(mMetadataLock);return mWelcomeMessage;}
    void setWelcomeMessage(std::optional<std::string> value){std::lock_guard lock(mMetadataLock);mWelcomeMessage=std::move(value);}
    int64_t getTimeConnected() const{std::lock_guard lock(mMetadataLock);return mTimeConnected;}
    int64_t getLastReadTime() const{std::lock_guard lock(mMetadataLock);return mLastMessageRead;}
    int32_t getConnectAttempts() const{std::lock_guard lock(mMetadataLock);return mConnectAttempts;}
    void incrementConnectAttempts(){std::lock_guard lock(mMetadataLock);mConnectAttempts=std::bit_cast<int32_t>(uint32_t(mConnectAttempts)+1);}
    void setConnectAttempts(int32_t value){std::lock_guard lock(mMetadataLock);mConnectAttempts=value;}
    void sendData(std::shared_ptr<MiniData> data);
    void handleRead();
    void handleWrite();
    void disconnect();
};
}
