#pragma once
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/objects/IBD.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
namespace minima {
struct TxPoWProcessorServices {
    std::function<void()> releaseLocks;
    std::function<bool()> isShuttingDown,isShuttingDownOrRestoring;
    std::function<void(std::shared_ptr<Message>)> postMain;
    std::function<void(const std::string&,const json::JSONObject&)> postNotifyEvent;
    std::function<void(bool)> setSyncIBD;
    // Defaults resolve the actual native singleton/manager at every source call.
    std::function<std::shared_ptr<MinimaDB>()> getDB=MinimaDB::getDB;
    std::function<void(std::shared_ptr<Message>)> postNIO;
    std::function<void(const std::optional<std::string>&,bool)> disconnect;
    std::function<void(const MiniByte&,const MiniData&)> sendAll;
    std::function<void(const std::optional<std::string>&,const MiniByte&,const MiniData&)> send;
    std::function<void(const std::string&)> log;
    std::function<int64_t()> now=TimerMessage::currentTimeMillis;
};
class TxPoWProcessor : public MessageProcessor {
    TxPoWProcessorServices mServices;
    std::atomic<int64_t> mFirstIBD;
    std::atomic<bool> mIBDSyncFinished{false};
    static constexpr int64_t MAX_FIRST_IBD_TIME=300000;
    void processTxPoW(std::shared_ptr<TxPoW> txpow);
    void processTxBlock(std::shared_ptr<TxBlock> block);
    bool processSyncBlock(std::shared_ptr<TxBlock> block);
    void recalculateTree();
    void processIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid,bool notifyFinish);
    void requestMissingTxns(const std::optional<std::string>& uid,const std::shared_ptr<TxBlock>& block);
protected:
    explicit TxPoWProcessor(TxPoWProcessorServices services);
    void processMessage(std::shared_ptr<Message> message) override;
public:
    static std::shared_ptr<TxPoWProcessor> create(TxPoWProcessorServices services);
    void resetFirstIBDTimer(){mFirstIBD=mServices.now();}
    void postProcessTxPoW(std::shared_ptr<TxPoW> txpow);
    void postProcessTxBlock(std::shared_ptr<TxBlock> block);
    void postProcessIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid,bool overrideRestore=false);
    bool isIBDProcessFinished() const {return mIBDSyncFinished.load();}
    void postProcessSyncIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid);
    void postProcessArchiveIBD(std::shared_ptr<IBD> ibd,const std::optional<std::string>& uid);
    void onStartUpRecalc(){recalculateTree();}
    void postCheckCall(){PostMessage("TXPOWPROCESSOR_CHECKER_CALL");}
    void askToSyncTxBlocks(const std::optional<std::string>& uid);
};
}
