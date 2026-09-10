#pragma once
#include "support/NIOMessageServices.hpp"
#include "support/NIOProtocol.hpp"
#include "support/JavaStringSet.hpp"
namespace minima {
class NIOManager;
class NIOMessage {
    std::optional<std::string> mClientUID;
    std::shared_ptr<MiniData> mData;
    bool mTrace=false;
    std::optional<std::string> mFilter="";
    static std::shared_ptr<NIOManager> manager();
    static std::shared_ptr<TxPowTree> tree();
    static int64_t now();
    void receiveGreeting(std::span<const uint8_t> data,size_t& offset);
    void receiveTxPoW(std::span<const uint8_t> data,size_t& offset);
    void receivePulse(std::span<const uint8_t> data,size_t& offset);
    void receivePing(std::span<const uint8_t> data,size_t& offset);
    static std::shared_ptr<TxBlock> lookupTxBlock(const std::string& id,bool archive);
public:
    static NIOMessageServices services;
    inline static JavaConcurrentMap<MiniNumber> mlastSyncReq;
    inline static JavaConcurrentMap<int64_t> mLastChainSync;
    inline static JavaStringSet mHaveSentIBDRecently;
    inline static std::atomic<int64_t> LAST_TXBLOCKMINE_MSG{0};
    int32_t HEAVIER_CHAIN_FOUND=0;
    std::optional<std::string> mFullAdrress="";
    inline static const MiniByte MSG_GREETING{0},MSG_IBD{1},MSG_TXPOWID{2},MSG_TXPOWREQ{3},MSG_TXPOW{4},MSG_GENMESSAGE{5},MSG_PULSE{6},MSG_P2P{7},MSG_PING{8},MSG_MAXIMA_CTRL{9},MSG_MAXIMA_TXPOW{10},MSG_SINGLE_PING{11},MSG_SINGLE_PONG{12},MSG_IBD_REQ{13},MSG_IBD_RESP{14},MSG_ARCHIVE_REQ{15},MSG_ARCHIVE_DATA{16},MSG_ARCHIVE_SINGLE_REQ{17},MSG_TXBLOCKID{18},MSG_TXBLOCKREQ{19},MSG_TXBLOCK{20},MSG_TXBLOCKMINE{21},MSG_MEGAMMRSYNC_REQ{22},MSG_MEGAMMRSYNC_RESP{23};
    NIOMessage(std::optional<std::string> uid,std::shared_ptr<MiniData> data):mClientUID(std::move(uid)),mData(std::move(data)){}
    static std::string convertMessageType(const MiniByte& type){return nioMessageType(type);}
    void setFullAddress(std::optional<std::string> address){mFullAdrress=std::move(address);}
    void setTrace(bool trace,std::optional<std::string> filter){mTrace=trace;mFilter=std::move(filter);}
    void run();
    // Direct binding for NIOManagerServices.processIncoming; no JVM or fallback.
    static void dispatch(std::optional<std::string> uid,std::shared_ptr<MiniData> data,bool trace,std::string filter,std::optional<std::string> host){NIOMessage message(std::move(uid),std::move(data));message.setTrace(trace,std::move(filter));message.setFullAddress(std::move(host));message.run();}
    friend class NIOMessageFixture;
};
}
