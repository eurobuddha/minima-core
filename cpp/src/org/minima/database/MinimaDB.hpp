#pragma once
#include "archive/ArchiveManager.hpp"
#include "archive/TxBlockDB.hpp"
#include "txpowdb/TxPoWDB.hpp"
#include "txpowtree/CoinDB.hpp"
#include "txpowtree/TxPowTree.hpp"
#include "userprefs/txndb/TxnDB.hpp"
#include "wallet/Wallet.hpp"
#include "org/minima/objects/mmr/MegaMMR.hpp"
#include "org/minima/system/network/p2p/P2PDB.hpp"
#include "org/minima/system/network/p2p2/P2P2DB.hpp"
#include "support/ReentrantReadWriteLock.hpp"
#include "support/IBDServices.hpp"
#include "support/ConsensusServices.hpp"
#include <set>
namespace minima {
struct MinimaDBServices {
    SqlDBServices sql;
    std::function<bool()> isShuttingDown;
    std::function<void(std::shared_ptr<Message>)> postMainMessage;
    std::function<void(std::shared_ptr<TimerMessage>)> postMainTimer;
    std::function<void(const std::string&,const json::JSONObject&)> postNotifyEvent;
    std::function<void(bool,const std::string&)> setStartupError;
    // Runtime.halt does not return; test drivers may throw out of this boundary.
    std::function<void(int)> halt;
    std::function<std::string()> currentThreadName;
};
class MinimaDB {
    friend struct WalletCommandsFixture;
    friend class PulseRuntimeFixture;
public:
    static void createDB(MinimaDBServices services);
    static std::shared_ptr<MinimaDB> getDB();
    explicit MinimaDB(MinimaDBServices services);
    virtual ~MinimaDB()=default;
    virtual void readLock(bool lock);
    virtual void writeLock(bool lock);
    virtual void safeReleaseReadWriteLock();
    virtual std::string getRWLockInfo(){return mRWLock.toString();}
    static std::string currentWriteLockThread();
    inline static std::atomic<bool> mCurrentWriteLockState{false};
    virtual std::shared_ptr<TxPoWDB> getTxPoWDB(){return owned(mTxPoWDB);}
    virtual std::shared_ptr<TxBlockDB> getTxBlockDB(){return owned(mTxBlockDB);}
    virtual std::shared_ptr<TxPowTree> getTxPoWTree(){return owned(mTxPoWTree);}
    virtual std::shared_ptr<MegaMMR> getMegaMMR(){return owned(mMegaMMR);}
    virtual void hardSetMegaMMR(std::shared_ptr<MegaMMR> mega){replace(mMegaMMR,std::move(mega));}
    virtual std::shared_ptr<Cascade> getCascade(){return owned(mCascade);}
    virtual void setIBDCascade(std::shared_ptr<Cascade> cascade){replace(mCascade,std::move(cascade));}
    virtual void resetCascadeAndTxPoWTree();
    virtual int64_t getCascadeFileSize(){return getDBFileSie("cascade.db");}
    virtual int64_t getUserDBFileSize(){return getDBFileSie("userprefs.db");}
    virtual int64_t getTxPowTreeFileSize(){return getDBFileSie("chaintree.db");}
    virtual int64_t getP2PFileSize(){return getDBFileSie("p2p.db");}
    virtual std::filesystem::path getCascadeFile(){return getDBFile("cascade.db");}
    virtual std::filesystem::path getDBFile(const std::optional<std::string>& name);
    virtual std::shared_ptr<UserDB> getUserDB(){return owned(mUserDB);}
    virtual std::shared_ptr<TxnDB> getCustomTxnDB(){return owned(mTxnDB);}
    virtual std::shared_ptr<Wallet> getWallet(){return owned(mWallet);}
    virtual std::shared_ptr<ArchiveManager> getArchive(){return owned(mArchive);}
    virtual std::shared_ptr<p2p::P2PDB> getP2PDB(){return owned(mP2PDB);}
    virtual std::shared_ptr<p2p2::P2P2DB> getP2P2DB(){return owned(mP2P2DB);}
    virtual std::filesystem::path getBaseDBFolder();
    virtual void loadAllDB();
    virtual void loadArchiveAndTxPoWDB(bool resetWallet);
    virtual void loadDBsForRestoreSync();
    virtual void refreshSQLDB();
    virtual void saveAllDB(){saveAllDB(false);}
    virtual void saveAllDB(bool compact);
    virtual void saveSQL(bool compact);
    virtual void fullDBRestartMemFree();
    virtual void setAllowSaveState(bool allow){mAllowSaveState=allow;}
    virtual void saveState();
    virtual void saveUserDB();
    virtual void saveP2PDB();
    virtual void addCoinNotify(const std::optional<std::string>& address);
    virtual bool removeCoinNotify(const std::optional<std::string>& address);
    virtual bool checkCoinNotify(const std::optional<std::string>& address);
    // Bind existing static native service boundaries after publishing the owner,
    // before starting workers. Callbacks resolve the current singleton each time.
    static void bindRuntime();
    static ChainServices chainServices();
    static IBDServices ibdServices();
    // Native dependency wiring, also used by temporary archive commands.
    std::shared_ptr<ArchiveManager> newArchive();
    static std::shared_ptr<IBDArchive> archiveServices(std::shared_ptr<ArchiveManager> archive);
    static ConsensusServices consensusServices();
protected:
    MinimaDBServices mServices;
    std::shared_ptr<ArchiveManager> mArchive;
    std::shared_ptr<TxPoWDB> mTxPoWDB;
    std::shared_ptr<TxPowTree> mTxPoWTree;
    std::shared_ptr<Cascade> mCascade;
    std::shared_ptr<UserDB> mUserDB;
    std::shared_ptr<TxnDB> mTxnDB;
    std::shared_ptr<Wallet> mWallet;
    std::shared_ptr<TxBlockDB> mTxBlockDB;
    std::shared_ptr<MegaMMR> mMegaMMR;
    std::shared_ptr<p2p::P2PDB> mP2PDB;
    std::shared_ptr<p2p2::P2P2DB> mP2P2DB;
    ReentrantReadWriteLock mRWLock;
    mutable std::mutex mOwners;
    template<class T> std::shared_ptr<T> owned(const std::shared_ptr<T>& field) const {std::lock_guard lock(mOwners);return field;}
    template<class T> void replace(std::shared_ptr<T>& field,std::shared_ptr<T> value){std::lock_guard lock(mOwners);field=std::move(value);}
private:
    inline static std::shared_ptr<MinimaDB> mMinimaDB;
    inline static std::mutex mInstanceLock,mDiagnosticLock;
    inline static std::string mCurrentWriteLockThread;
    std::set<std::optional<std::u16string>> mCoinNotify;
    std::mutex mNotifyLock;
    std::atomic<bool> mAllowSaveState{true};
    std::shared_ptr<TxPoWDB> newTxPoWDB();
    void loadWallet(const std::filesystem::path& base,bool announceEncryption);
    int64_t getDBFileSie(const std::string& name);
    void log(const std::string& text) const;
};
}
