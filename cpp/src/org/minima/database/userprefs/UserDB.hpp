#pragma once
#include "org/minima/utils/JsonDB.hpp"
#include "org/minima/utils/MiniUtil.hpp"
namespace minima {
class UserDB : public JsonDB {
    static std::string normalizedName(const std::string& name);
public:
    // Empty uses ICU's process locale; the native runtime may set the Java language explicitly.
    static std::string language;
    UserDB()=default;
    void setWelcome(const std::optional<std::string>& zWelcome);
    std::string getWelcome();
    std::string getIncentiveCashUserID();
    void setIncentiveCashUserID(const std::optional<std::string>& zUID);
    void setWebHooks(const MiniUtil::StringList& zWebHooks);
    MiniUtil::StringList getWebHooks();
    void setWebHooksUTF16(const MiniUtil::UTF16StringList& hooks);
    MiniUtil::UTF16StringList getWebHooksUTF16();
    MiniData loadCustomTransactions();
    void saveCustomTransactions(const MiniData& zCompleteDB);
    void setHashRate(const MiniNumber& zHashesPerSec);
    MiniNumber getHashRate();
    void setMaximaName(const std::optional<std::string>& zName);
    std::string getMaximaName();
    void setMaximaIcon(const std::optional<std::string>& zIcon);
    std::string getMaximaIcon();
    bool getMDSINIT();
    void setMDSINIT(bool zInit);
    bool isAutoBackup();
    void setAutoBackup(bool zAuto);
    MiniNumber getMagicDesiredKISSVM();
    void setMagicDesiredKISSVM(const MiniNumber& zKISSVM);
    MiniNumber getMagicMaxTxPoWSize();
    void setMagicMaxTxPoWSize(const MiniNumber& zMaxSize);
    MiniNumber getMagicMaxTxns();
    void setMagicMaxTxns(const MiniNumber& zMaxTxns);
    void setEncryptedSeed(const MiniData& zEncryptedSeed);
    MiniData getEncryptedSeed();
    bool getMaximaAllowContacts();
    void setMaximaAllowContacts(bool zAllowContacts);
    MiniUtil::StringList getMaximaPermanent();
    void setMaximaPermanent(const MiniUtil::StringList& zPermanentList);
    bool getAutoLoginDetailsMySQL();
    void setAutoLoginDetailsMySQL(bool zLoginDetails);
    bool getAutoBackupMySQL();
    void setAutoBackupMySQL(bool zAuto);
    bool getAutoBackupMySQLCoins();
    void setAutoBackupMySQLCoins(bool zAuto);
    void setAutoMySQLHost(const std::optional<std::string>& zHost);
    std::string getAutoMySQLHost();
    void setAutoMySQLDB(const std::optional<std::string>& zDB);
    std::string getAutoMySQLDB();
    void setAutoMySQLUser(const std::optional<std::string>& zUser);
    std::string getAutoMySQLUser();
    void setAutoMySQLPassword(const std::optional<std::string>& zPassword);
    std::string getAutoMySQLPassword();
    bool isSlaveNode();
    std::string getSlaveNodeHost();
    void setSlaveNode(bool zEnabled,const std::optional<std::string>& zHost);
    std::string getDefaultMiniHUB();
    void setDefaultMiniHUB(const std::optional<std::string>& zMiniDAPPID);
    void clearUninstalledMiniDAPP();
    std::shared_ptr<json::JSONArray> getUninstalledMiniDAPP();
    void removeUninstalledMiniDAPP(const std::optional<std::string>& zName);
    void addUninstalledMiniDAPP(const std::optional<std::string>& zName);
    bool checkUninstalledMiniDAPP(const std::optional<std::string>& zName);
    void setPublicMDS(bool zEnable);
    bool getPublicMDS();
    std::shared_ptr<json::JSONArray> getRPCUsers();
    void setRPCUsers(const std::shared_ptr<json::JSONArray>& zNewUsers);
};
}
