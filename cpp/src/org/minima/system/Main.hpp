#pragma once
#include "org/minima/database/MinimaDB.hpp"
#include "brains/TxPoWProcessor.hpp"
#include "brains/TxPoWMiner.hpp"
#include "network/NetworkManager.hpp"
#include "network/webhooks/NotifyManager.hpp"
#include "commands/sendpoll/SendPollManager.hpp"
#include "commands/backup/vault.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/NIOMessageServices.hpp"
namespace minima {
struct MainServices {
    SqlDBServices sql;
    MySQLConnectServices mysql;
    std::function<int64_t()> now=TimerMessage::currentTimeMillis;
    std::function<void(int64_t)> sleep;
    std::function<void()> loadNativeLibrary,collectMemory;
    std::function<void(int)> halt,exit;
    std::function<std::string()> currentThreadName;
    // Supplied by the megammrsync command's native runtime when available.
    std::function<std::shared_ptr<MegaMMRIBD>(const MegaMMRSyncData&)> getCurrentMegaMMRIBD;
};
class Main:public MessageProcessor,public MinimaLogger::MainReceiver,public commands::vault::MainReceiver {
public:
    inline static bool STARTUP_DEBUG_LOGS=false;
    std::atomic<bool> STARTUP_ERROR{false},STARTUP_COMPLETE{false},MYSQL_IMPORTING_NO_ACTION{false};
    std::optional<std::string> STARTUP_ERROR_MSG=std::string();
    std::atomic<int64_t> AUTOMINE_TIMER{50000};
    inline static const std::string MAIN_TXPOWMINED="MAIN_TXPOWMINED",MAIN_PULSE="MAIN_PULSE",MAIN_CLEANDB_RAM="MAIN_CLEANDB_RAM",MAIN_CLEANDB_SQL="MAIN_CLEANDB_SQL",MAIN_SYSTEMCLEAN="MAIN_SYSTEMCLEAN",MAIN_AUTOBACKUP_MYSQL="MAIN_AUTOBACKUP_MYSQL",MAIN_AUTOBACKUP_TXPOW="MAIN_AUTOBACKUP_TXPOW",MAIN_DO_RESCUE="MAIN_DO_RESCUE",MAIN_LOAD_ALL_KEYS="MAIN_LOAD_ALL_KEYS",MAIN_AUTOBACKUP="MAIN_AUTOBACKUP",MAIN_SHUTDOWN="MAIN_SHUTDOWN",MAIN_NETRESTART="MAIN_NETRESTART",MAIN_NETRESET="MAIN_NETRESET",MAIN_CHECKER="MAIN_CHECKER",MAIN_INIT_KEYS="MAIN_INIT_KEYS",MAIN_CALLCHECKER="MAIN_CALLCHECKER",MAIN_P2PNETMDS_CHECKER="MAIN_P2PNETMDS_CHECKER",MAIN_NEWBLOCK="MAIN_NEWBLOCK",MAIN_BALANCE="MAIN_BALANCE",MAIN_MINING="MAIN_MINING",MAIN_NEWCOIN="NEWCOIN",MAIN_NOTIFYCOIN="NOTIFYCOIN",MAIN_NOTIFYCASCADEBLOCK="NOTIFYCASCADEBLOCK",MAIN_NOTIFYCASCADETXN="NOTIFYCASCADETXN",MAIN_NOTIFYCASCADECOIN="NOTIFYCASCADECOIN";
    static std::shared_ptr<Main> create(MainServices services);
    static std::shared_ptr<Main> getInstance();
    static void ClearMainInstance();
    static std::shared_ptr<MessageListener> getMinimaListener();
    static void setMinimaListener(std::shared_ptr<MessageListener> listener);
    static void NotifyMainListenerOnly(const std::optional<std::string>& event);
    static void NotifyMainListenerOnly(const std::optional<std::string>& event,std::shared_ptr<json::JSONObject> data);
    ~Main() override;
    bool isStartUpComplete() const{return STARTUP_COMPLETE;}
    json::JSONObject runSingleMinimaCMD(const std::optional<std::string>& input);
    void setSyncIBD(bool sync);
    bool isSyncIBD() const{return mSyncIBD;}
    void setHasShutDown() override{mShuttingdown=true;}
    bool isShuttingDown() const{return mShuttingdown;}
    bool isRestoring() const{return mRestoring;}
    bool isShuttongDownOrRestoring() const{return mShuttingdown||mRestoring;}
    void shutdown(bool compact=false);
    void NotifyMainListenerOfShutDown();
    void setStartUpError(bool error,const std::optional<std::string>& message);
    bool isStartupError() const{return STARTUP_ERROR;}
    std::optional<std::string> getStartupErrorMsg() const;
    void restoreReady() override{restoreReady(true);}
    virtual void restoreReady(bool shutdownMDS);
    void restoreReadyForSync();
    void archiveResetReady(bool resetWallet,bool resetCascadeTree=true);
    void shutdownFinalProcs(bool shutdownMDS=true);
    void resetMemFull();
    void restartNIO();
    void setNormalAutoMineSpeed(){mNormalMineMode=true;AUTOMINE_TIMER=50000;}
    void setLowPowAutoMineSpeed(){mNormalMineMode=false;AUTOMINE_TIMER=500000;}
    bool isNormalMineMode() const{return mNormalMineMode;}
    int64_t getUptimeMilli() const;
    std::shared_ptr<NetworkManager> getNetworkManager() const{return owner(mNetwork);}
    std::shared_ptr<NIOManager> getNIOManager() const;
    std::shared_ptr<NotifyManager> getNotifyManager() const{return owner(mNotifyManager);}
    std::shared_ptr<TxPoWProcessor> getTxPoWProcessor() const{return owner(mTxPoWProcessor);}
    std::shared_ptr<TxPoWMiner> getTxPoWMiner() const{return owner(mTxPoWMiner);}
    std::shared_ptr<SendPollManager> getSendPoll() const{return owner(mSendPoll);}
    bool getAllKeysCreated() override{return mInitKeysCreated;}
    int32_t getAllDefaultKeysSize() override;
    void stopMessageProcessor() override{MessageProcessor::stopMessageProcessor();}
    void PostNotifyEvent(const std::string& event,std::shared_ptr<json::JSONObject> data) override;
    void PostNotifyEvent(const std::optional<std::string>& event,std::shared_ptr<json::JSONObject> data,const std::optional<std::string>& to);
    void PostNotifyEvent(const std::string& event,const json::JSONObject& data){PostNotifyEvent(event,std::make_shared<json::JSONObject>(data));}
protected:
    explicit Main(MainServices services);
    void processMessage(std::shared_ptr<Message> message) override;
private:
    MainServices mServices;
    std::shared_ptr<class TimerProcessor> mTimerOwner;
    int64_t mUptimeMilli;
    std::atomic<bool> mShutDownSentToListener{false},mNormalMineMode{true},mShuttingdown{false},mHaveShutDownMDS{false},mRestoring{false},mSyncIBD{false},mInitKeysCreated{false};
    int64_t CLEANDB_RAM_TIMER=1800000,CLEANDB_SQL_TIMER=43200000,SYSTEMCLEAN_TIMER=300000,MAIN_AUTOBACKUP_MYSQL_TIMER=7200000,AUTOBACKUP_TIMER=86400000,NETRESET_TIMER=86400000,INIT_KEYS_TIMER=10000,CHECKER_TIMER=180000,P2PNETMDS_TIMER=1800000;
    MiniData mOldTip=MiniData::ZERO_TXPOWID;
    std::shared_ptr<TxPoWProcessor> mTxPoWProcessor;
    std::shared_ptr<TxPoWMiner> mTxPoWMiner;
    std::shared_ptr<NetworkManager> mNetwork;
    std::shared_ptr<SendPollManager> mSendPoll;
    std::shared_ptr<NotifyManager> mNotifyManager;
    mutable std::mutex mOwners,mErrorLock;
    inline static std::mutex mStaticLock;
    inline static std::shared_ptr<Main> mMainInstance;
    inline static std::shared_ptr<MessageListener> MINIMA_LISTENER;
    template<class T> std::shared_ptr<T> owner(const std::shared_ptr<T>& value) const{std::lock_guard lock(mOwners);return value;}
    template<class T> void replace(std::shared_ptr<T>& field,std::shared_ptr<T> value){{std::lock_guard lock(mOwners);field.swap(value);} /* Retire outside the lock: workers can reenter Main. */}
    void initialize();
    void bindRuntime();
    void doGenesis();
    void shutdownGenProcs();
    void cleanNativeOwners() noexcept;
    void collectMemory(){if(mServices.collectMemory)mServices.collectMemory();} // JVM GC is a hint; native objects use shared/RAII ownership.
    void schedule(int64_t delay,const std::string& type){PostTimerMessage(std::make_shared<TimerMessage>(delay,type));}
    std::shared_ptr<TxPoWProcessor> newProcessor();
    std::shared_ptr<NetworkManager> newNetwork();
    MinimaDBServices databaseServices();
    friend class MainFixture;
};
}
