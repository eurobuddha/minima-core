#include "UserDB.hpp"
#include "org/minima/objects/Magic.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/JavaUnicode.hpp"
#include "support/JavaLowerCase.hpp"
#include <unicode/ustring.h>
#include <algorithm>
namespace minima {
std::string UserDB::language;
std::string UserDB::normalizedName(const std::string& name) {
    auto lowered=javaLowerCase(name,language);std::erase(lowered,' ');return lowered;
}
void UserDB::setWelcome(const std::optional<std::string>& zWelcome) {setString("welcome", zWelcome);}
std::string UserDB::getWelcome() {return getString("welcome", "Running Minima "+GlobalParams::MINIMA_VERSION);}
std::string UserDB::getIncentiveCashUserID() {return getString("uid", "");}
void UserDB::setIncentiveCashUserID(const std::optional<std::string>& zUID) {setString("uid", zUID);}
void UserDB::setWebHooks(const MiniUtil::StringList& zWebHooks) {setJSONArray("webhooks",MiniUtil::convertArrayList(zWebHooks));}
MiniUtil::StringList UserDB::getWebHooks() {return MiniUtil::convertJSONArray(getJSONArray("webhooks"));}
void UserDB::setWebHooksUTF16(const MiniUtil::UTF16StringList& hooks) {setJSONArray("webhooks",MiniUtil::convertUTF16ArrayList(hooks));}
MiniUtil::UTF16StringList UserDB::getWebHooksUTF16() {return MiniUtil::convertUTF16JSONArray(getJSONArray("webhooks"));}
MiniData UserDB::loadCustomTransactions() {return getData("custom_transactions", MiniData::ZERO_TXPOWID);}
void UserDB::saveCustomTransactions(const MiniData& zCompleteDB) {setData("custom_transactions", zCompleteDB);}
void UserDB::setHashRate(const MiniNumber& zHashesPerSec) {setNumber("hashrate", zHashesPerSec);}
MiniNumber UserDB::getHashRate() {return getNumber("hashrate", MiniNumber::MILLION);}
void UserDB::setMaximaName(const std::optional<std::string>& zName) {setString("maximaname", zName);}
std::string UserDB::getMaximaName() {return getString("maximaname", "noname");}
void UserDB::setMaximaIcon(const std::optional<std::string>& zIcon) {setString("maximaicon", zIcon);}
std::string UserDB::getMaximaIcon() {return getString("maximaicon", "0x00");}
bool UserDB::getMDSINIT() {return getBoolean("mdsinitdapps", false);}
void UserDB::setMDSINIT(bool zInit) {setBoolean("mdsinitdapps", zInit);}
bool UserDB::isAutoBackup() {return getBoolean("autobackup", false);}
void UserDB::setAutoBackup(bool zAuto) {setBoolean("autobackup", zAuto);}
MiniNumber UserDB::getMagicDesiredKISSVM() {return getNumber("magic_kissvm", Magic::DEFAULT_KISSVM_OPERATIONS);}
void UserDB::setMagicDesiredKISSVM(const MiniNumber& zKISSVM) {setNumber("magic_kissvm", zKISSVM);}
MiniNumber UserDB::getMagicMaxTxPoWSize() {return getNumber("magic_txpowsize", Magic::DEFAULT_TXPOW_SIZE);}
void UserDB::setMagicMaxTxPoWSize(const MiniNumber& zMaxSize) {setNumber("magic_txpowsize", zMaxSize);}
MiniNumber UserDB::getMagicMaxTxns() {return getNumber("magic_txns", Magic::DEFAULT_TXPOW_TXNS);}
void UserDB::setMagicMaxTxns(const MiniNumber& zMaxTxns) {setNumber("magic_txns", zMaxTxns);}
void UserDB::setEncryptedSeed(const MiniData& zEncryptedSeed) {setData("encrypted_seed", zEncryptedSeed);}
MiniData UserDB::getEncryptedSeed() {return getData("encrypted_seed", MiniData::ZERO_TXPOWID);}
bool UserDB::getMaximaAllowContacts() {return getBoolean("maxima_allowallcontacts", true);}
void UserDB::setMaximaAllowContacts(bool zAllowContacts) {setBoolean("maxima_allowallcontacts", zAllowContacts);}
MiniUtil::StringList UserDB::getMaximaPermanent() {return MiniUtil::convertJSONArray(getJSONArray("maxima_permanent"));}
void UserDB::setMaximaPermanent(const MiniUtil::StringList& zPermanentList) {setJSONArray("maxima_permanent",MiniUtil::convertArrayList(zPermanentList));}
bool UserDB::getAutoLoginDetailsMySQL() {return getBoolean("mysql_autologindetails", false);}
void UserDB::setAutoLoginDetailsMySQL(bool zLoginDetails) {setBoolean("mysql_autologindetails", zLoginDetails);}
bool UserDB::getAutoBackupMySQL() {return getBoolean("mysql_autobackup", false);}
void UserDB::setAutoBackupMySQL(bool zAuto) {setBoolean("mysql_autobackup", zAuto);}
bool UserDB::getAutoBackupMySQLCoins() {return getBoolean("mysqlcoins_autobackup", false);}
void UserDB::setAutoBackupMySQLCoins(bool zAuto) {setBoolean("mysqlcoins_autobackup", zAuto);}
void UserDB::setAutoMySQLHost(const std::optional<std::string>& zHost) {setString("mysql_host", zHost);}
std::string UserDB::getAutoMySQLHost() {return getString("mysql_host","");}
void UserDB::setAutoMySQLDB(const std::optional<std::string>& zDB) {setString("mysql_db", zDB);}
std::string UserDB::getAutoMySQLDB() {return getString("mysql_db","");}
void UserDB::setAutoMySQLUser(const std::optional<std::string>& zUser) {setString("mysql_user", zUser);}
std::string UserDB::getAutoMySQLUser() {return getString("mysql_user","");}
void UserDB::setAutoMySQLPassword(const std::optional<std::string>& zPassword) {setString("mysql_password", zPassword);}
std::string UserDB::getAutoMySQLPassword() {return getString("mysql_password","");}
bool UserDB::isSlaveNode() {return getBoolean("slavenode_enabled", false);}
std::string UserDB::getSlaveNodeHost() {return getString("slavenode_host", "");}
void UserDB::setSlaveNode(bool zEnabled,const std::optional<std::string>& zHost) {setBoolean("slavenode_enabled", zEnabled); setString("slavenode_host", zHost);}
std::string UserDB::getDefaultMiniHUB() {return getString("minihub_default", "0x00");}
void UserDB::setDefaultMiniHUB(const std::optional<std::string>& zMiniDAPPID) {setString("minihub_default", zMiniDAPPID);}
void UserDB::clearUninstalledMiniDAPP() {setJSONArray("minidapps_uninstalled",std::make_shared<json::JSONArray>());}
std::shared_ptr<json::JSONArray> UserDB::getUninstalledMiniDAPP() {return getJSONArray("minidapps_uninstalled");}
void UserDB::removeUninstalledMiniDAPP(const std::optional<std::string>& zName) {auto list=getUninstalledMiniDAPP();list->removeString(normalizedName(zName.value()));setJSONArray("minidapps_uninstalled",list);}
void UserDB::addUninstalledMiniDAPP(const std::optional<std::string>& zName) {auto list=getUninstalledMiniDAPP();auto name=normalizedName(zName.value());list->removeString(name);list->add(name);setJSONArray("minidapps_uninstalled",list);}
bool UserDB::checkUninstalledMiniDAPP(const std::optional<std::string>& zName) {return getUninstalledMiniDAPP()->containsString(normalizedName(zName.value()));}
void UserDB::setPublicMDS(bool zEnable) {setBoolean("mds_publicsite", zEnable);}
bool UserDB::getPublicMDS() {if(GeneralParams::PUBLICMDS_ENABLE)return true;return getBoolean("mds_publicsite",false);}
std::shared_ptr<json::JSONArray> UserDB::getRPCUsers() {return getJSONArray("rpcusers_allusers");}
void UserDB::setRPCUsers(const std::shared_ptr<json::JSONArray>& zNewUsers) {setJSONArray("rpcusers_allusers", zNewUsers);}
}
