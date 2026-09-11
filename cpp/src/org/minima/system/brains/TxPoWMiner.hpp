#pragma once
#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
struct TxPoWMinerServices {
    std::function<void()> releaseLocks;
    std::function<void(std::shared_ptr<Message>)> postMain;
    std::function<int64_t()> automineTimer;
    std::function<std::shared_ptr<TxPoWTreeNode>()> getTip;
    std::function<std::shared_ptr<TxPoW>()> generate;
    std::function<void(const MiniByte&,std::shared_ptr<TxPoW>)> sendAll;
    std::function<void(const std::string&)> log;
    std::function<int64_t()> now=TimerMessage::currentTimeMillis;
};
class TxPoWMiner : public MessageProcessor {
    TxPoWMinerServices mServices;
    mutable std::mutex mCoinsLock;
    std::vector<std::string> mMiningCoins;
    void addMiningCoins(const std::shared_ptr<TxPoW>& txpow);
    void removeMiningCoins(const std::shared_ptr<TxPoW>& txpow);
protected:
    explicit TxPoWMiner(TxPoWMinerServices services);
    void processMessage(std::shared_ptr<Message> message) override;
public:
    static void main(const std::vector<std::string>* args);
    inline static const std::string TXPOWMINER_MINETXPOW="TXPOWMINER_MINETXPOW",
        TXPOWMINER_MINEPULSE="TXPOWMINER_MINEPULSE",TXPOWMINER_TXBLOCKMINER="TXPOWMINER_TXBLOCKMINER";
    static std::shared_ptr<TxPoWMiner> create(TxPoWMinerServices services);
    virtual void mineTxPoWAsync(std::shared_ptr<TxPoW> txpow);
    bool checkForMiningCoin(const std::optional<std::string>& coinID) const;
    virtual bool MineMaxTxPoW(bool maxima,std::shared_ptr<TxPoW> txpow,int64_t timeLimit,bool post=true);
    // Optional clock is the same explicit replay boundary used by the generator.
    static MiniNumber calculateHashRateOld(const MiniNumber& hashes,
        const std::function<int64_t()>& now=TimerMessage::currentTimeMillis);
    static MiniNumber calculateHashSpeed(const MiniNumber& hashes,
        const std::function<int64_t()>& now=TimerMessage::currentTimeMillis);
};
}
