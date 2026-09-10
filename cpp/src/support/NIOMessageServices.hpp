#pragma once
#include "org/minima/objects/IBD.hpp"
#include "org/minima/system/commands/backup/mmrsync/MegaMMRIBD.hpp"
#include "org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.hpp"
#include "org/minima/utils/MiniUtil.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
// External receivers of the pinned NIOMessage. Decoding, protocol decisions,
// IBD creation, consensus validation and chain searches use actual native code.
// Full Main/MinimaDB/P2P/processor/miner bindings remain required at startup.
struct NIOMessageServices {
    std::function<bool()> isShuttingDownOrRestoring,isSyncIBD;
    std::function<std::shared_ptr<TxPowTree>()> getTree;
    std::function<std::shared_ptr<Cascade>()> getCascade;
    std::function<bool(const std::string&)> txPoWExists;
    std::function<std::shared_ptr<TxPoW>(const std::string&)> getTxPoW;
    std::function<std::shared_ptr<TxBlock>(const std::string&)> findTxBlock,loadArchiveBlock;
    std::function<void(const std::string&)> addInvalidPeer;
    std::function<void(std::shared_ptr<Message>)> postP2P;
    std::function<void(std::shared_ptr<TimerMessage>)> postP2PTimer,postMainTimer;
    std::function<void(const std::string&,const json::JSONObject&)> notifyEvent;
    std::function<void(std::shared_ptr<IBD>,std::optional<std::string>)> postProcessIBD,postProcessSyncIBD;
    std::function<void(std::shared_ptr<TxPoW>)> postProcessTxPoW,mineTxPoWAsync;
    std::function<void(std::shared_ptr<TxBlock>)> postProcessTxBlock;
    std::function<SocketAddressList()> getPeersCopy;
    std::function<float()> getP2PClients;
    std::function<std::shared_ptr<MiniUtil::StringList>()> getP2P2Peers;
    std::function<std::shared_ptr<MegaMMRIBD>(const MegaMMRSyncData&)> getCurrentMegaMMRIBD;
    std::function<int64_t()> getAutomineTimer,currentTimeMillis;
};
}
