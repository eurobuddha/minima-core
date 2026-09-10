#pragma once
#include "org/minima/utils/SqlDB.hpp"
#include "KeyRow.hpp"
#include "SeedRow.hpp"
#include "ScriptRow.hpp"
#include "org/minima/objects/keys/Signature.hpp"
#include <atomic>
#include <set>
namespace minima {
class Wallet : public SqlDB {
    friend struct WalletVaultFixture;
public:
    using Text=std::optional<std::string>;
    using Keys=std::vector<std::shared_ptr<KeyRow>>;
    using Scripts=std::vector<std::shared_ptr<ScriptRow>>;
    inline static std::atomic<int32_t> NUMBER_GETADDRESS_KEYS{64};
    explicit Wallet(SqlDBServices services);
    virtual void shuttingDown();
    virtual void resetDB(const Text& phrase);
    virtual void updateSeedRow(const Text& phrase,const Text& seed);
    virtual std::shared_ptr<SeedRow> getBaseSeed();
    virtual bool isBaseSeedAvailable();
    virtual void wipeBaseSeedRow();
    virtual bool resetBaseSeedPrivKeys(const Text& phrase,const Text& seed);
    virtual bool checkAllPrivateKeys();
    virtual bool checkSingleKey(const Text& privateKey,const Text& modifier);
    virtual void setStopNewKeys(bool stop);
    virtual int32_t getDefaultKeysNumber();
    virtual bool initDefaultKeys(int32_t max);
    virtual bool initDefaultKeys(int32_t max,bool logs);
    virtual std::shared_ptr<ScriptRow> getDefaultAddress();
    virtual std::shared_ptr<ScriptRow> createNewSimpleAddress(bool isDefault);
    virtual std::shared_ptr<KeyRow> createNewKey();
    virtual std::shared_ptr<ScriptRow> addScript(const Text& script,bool simple,bool isDefault,const Text& publicKey,bool track);
    virtual void removeScript(const Text& address);
    virtual std::shared_ptr<ScriptRow> getScriptFromAddress(const Text& address);
    virtual bool isKeyRelevant(const Text& key);
    virtual bool isAddressRelevant(const Text& address);
    virtual bool isAddressSimple(const Text& address);
    virtual Keys getAllKeys();
    virtual std::shared_ptr<KeyRow> getKeyFromPublic(const Text& publicKey);
    virtual Scripts getAllAddresses();
    virtual std::shared_ptr<Signature> signData(const Text& publicKey,const MiniData* data);
    std::shared_ptr<Signature> signData(const Text& publicKey,const MiniData& data){return signData(publicKey,&data);}
    virtual void updateAllKeyUses(int32_t uses);
    virtual void updateIncrementAllKeyUses(int32_t uses);
protected:
    void createSQL() override;
    std::shared_ptr<SeedRow> mBaseSeed;
    std::set<Text> mAllKeys,mAllTrackedAddress,mAllSimpleAddress;
    std::atomic<bool> mShuttingdown{false},STOP_NEWKEYS{false};
private:
    std::shared_ptr<SqlPreparedStatement> SQL_CREATE_PUBLIC_KEY,SQL_GET_KEY,SQL_GET_ALL_KEYS,
        SQL_UPDATE_KEY_USES,SQL_UPDATE_ALL_KEY_USES,SQL_UPDATE_INC_ALL_KEY_USES,
        SQL_WIPE_PRIVATE_KEYS,SQL_UPDATE_PRIVATE_KEYS,SQL_ADD_SCRIPT,SQL_REMOVE_SCRIPT,
        SQL_LIST_ALL_SCRIPTS,SQL_LIST_SIMPLE_SCRIPTS,SQL_LIST_TRACK_SCRIPTS,
        SQL_LIST_DEFAULT_SCRIPTS,SQL_GET_SCRIPT,SQL_SELECT_SEED,SQL_INSERT_SEED,SQL_UPDATE_SEED;
    bool isShuttingDown();
    void initBaseSeed();
    Scripts getAllDefaultAddresses();
    void updateUses(int32_t uses,const Text& publicKey);
};
}
