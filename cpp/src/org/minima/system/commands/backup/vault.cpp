#include "vault.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/encrypt/PasswordCrypto.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaStringIgnoreCase.hpp"
#include <thread>
namespace minima::commands {
namespace {const std::string& requiredText(const std::optional<std::string>& text){if(!text)throw JavaNullPointerException("null vault string");return *text;}}
std::function<std::shared_ptr<vault::MainReceiver>()> vault::getMain=[](){return std::shared_ptr<MainReceiver>{};};
vault::vault():Command("vault","[action:seed|wipekeys|restorekeys|passwordlock|passwordunlock] (seed:) (phrase:) - BE CAREFUL. Wipe/Restore/Encrypt/Decrypt your private keys"){}
std::optional<std::string> vault::getFullHelp() const {return std::string(
"\nvault\n"
"\n"
"BE CAREFUL. Wipe / Restore your private keys.\n"
"\n"
"View your passphrase and seed. DO NOT SHARE THESE WITH ANYONE.\n"
"\n"
"ENSURE YOU HAVE A BACKUP AND SECURE RECORD OF YOUR PASSPHRASE BEFORE LOCKING.\n"
"\n"
"You must have your passphrase to unlock your private keys.\n"
"\n"
"action: (optional)\n"
"    seed : Show your seed phrase. The default. \n"
"    wipekeys : Wipe your private keys - keep the public.\n"
"    restorekeys : Restore your private keys.\n"
"    passwordlock : Lock your node by password encrypting private keys.\n"
"    passwordunlock : Unlock your node to reinstating your private keys.\n"
"\n"
"seed: (optional)\n"
"    Enter your seed to lock your node.\n"
"    This will delete your private keys.\n"
"\n"
"phrase: (optional)\n"
"    Enter your passphrase in double quotes to restore your node.\n"
"    This will reinstate your private keys.\n"
"\n"
"Examples:\n"
"\n"
"vault\n"
"\n"
"vault action:wipekeys seed:0xDD4E..\n"
"\n"
"vault action:restorekeys phrase:\"SPRAY LAMP..\"\n"
"\n"
"vault action:passwordlock password:your_password\n"
"\n"
"vault action:passwordlock password:your_password confirm:your_password\n"
"\n"
"vault action:passwordunlock password:your_password\n");}
std::vector<std::string> vault::getValidParams() const {return {"action","seed","keyuses","phrase","password","confirm","numkeys"};}
std::shared_ptr<Command> vault::getFunction() const {return std::make_shared<vault>();}

json::JSONValue vault::runCommand(){
    auto result=getJSONReply();auto action=getParam("action","seed");
    auto base=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getBaseSeed();
    if(action=="seed"){
        json::JSONObject value;value.put("phrase",sqlRequired(base).getPhrase()?json::JSONValue(*base->getPhrase()):json::JSONValue());
        value.put("seed",base->getSeed()?json::JSONValue(*base->getSeed()):json::JSONValue());
        value.put("locked",!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable());result.put("response",value);
    }else if(action=="wipekeys"){
        checkAllKeysCreated();if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("DB already locked!");
        auto seed=getDataParam("seed");if(!sqlRequired(base).getSeed()||seed.to0xString()!=*base->getSeed())throw CommandException("Incorrect seed for lock");
        sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).wipeBaseSeedRow();result.put("response","All private keys wiped!");
    }else if(action=="restorekeys"){
        auto phrase=getParam("phrase");auto cleaned=BIP39::cleanSeedPhrase(phrase);auto seed=BIP39::convertStringToSeed(cleaned);
        if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).resetBaseSeedPrivKeys(cleaned,seed.to0xString()))throw CommandException("Error updating Private keys.. please try again");
        json::JSONObject value;value.put("entered",phrase);value.put("cleaned",cleaned);value.put("same",javaStringEqualsIgnoreCase(cleaned,phrase));value.put("result","All private keys restored!");result.put("response",value);
    }else if(action=="passwordlock"){
        checkAllKeysCreated();auto password=getParam("password");if(password.find(';')!=std::string::npos)throw CommandException("Cannot use ; in password");
        if(existsParam("confirm")&&password!=getParam("confirm"))throw CommandException("Passwords do NOT match!");
        passwordLockDB(password);result.put("response","All private keys wiped! Stored encrypted in UserDB");
    }else if(action=="passwordunlock"){
        passowrdUnlockDB(getParam("password"));result.put("response","All private keys restored!");
    }else if(action=="testphrase"){
        auto phrase=getParam("phrase");auto cleaned=BIP39::cleanSeedPhrase(phrase);auto seed=BIP39::convertStringToSeed(cleaned);auto number=getNumberParam("numkeys",MiniNumber(4)).getAsInt();json::JSONArray addresses;
        for(int i=0;i<number;++i){MinimaLogger::log("Creating key : "+std::to_string(i));MiniData modifier{BigInt(int64_t(i))};auto privateSeed=Crypto::getInstance().hashObjects(seed,modifier);auto tree=TreeKey::createDefault(privateSeed);
            auto publicKey=tree.getPublicKey();if(!publicKey)throw JavaNullPointerException("null public key");Address address("RETURN SIGNEDBY("+publicKey->to0xString()+")");json::JSONObject value;value.put("publickey",publicKey->to0xString());value.put("address",address.getMinimaAddress());addresses.add(value);}
        json::JSONObject value;value.put("phrase",cleaned);value.put("seed",seed.to0xString());value.put("address",addresses);result.put("response",value);
    }else if(action=="resetkeys"){
        auto phrase=getParam("phrase");auto uses=getNumberParam("keyuses",MiniNumber(100)).getAsInt();auto cleaned=BIP39::cleanSeedPhrase(phrase);
        sqlRequired(getMain()).restoreReady();
        sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB()).wipeDB();
        sqlRequired(sqlRequired(sqlRequired(MinimaDB::getDB()).getTxPoWDB()).getSQLDB()).saveDB(false);
        sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).saveDB(false);
        auto archive=sqlRequired(sqlRequired(MinimaDB::getDB()).getArchive()).getSQLFile();if(!archive)throw JavaNullPointerException("null archive file");
        if(archive->string().find('\0')==std::string::npos){std::error_code error;std::filesystem::remove(*archive,error);}
        auto folder=sqlRequired(MinimaDB::getDB()).getBaseDBFolder();
        for(const auto* name:{"cascade.db","chaintree.db","userprefs.db","p2p.db"})MiniFile::deleteFileOrFolder(GeneralParams::DATA_FOLDER,folder/name);
        auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();sqlRequired(wallet).resetDB(cleaned);wallet->initDefaultKeys(Wallet::NUMBER_GETADDRESS_KEYS.load(),true);wallet->updateAllKeyUses(uses);
        sqlRequired(MinimaDB::getDB()).saveSQL(false);sqlRequired(getMain()).setHasShutDown();sqlRequired(getMain()).stopMessageProcessor();result.put("response","All private keys restored!");
    }else throw CommandException("Invalid action : "+action);
    return result;
}
void vault::checkAllKeysCreated(){if(sqlRequired(getMain()).getAllKeysCreated())return;
    auto size=sqlRequired(getMain()).getAllDefaultKeysSize();throw CommandException("Please wait for ALL your keys to be created. This can take 5 mins. Currently ("+std::to_string(size)+"/"+std::to_string(Wallet::NUMBER_GETADDRESS_KEYS.load())+")");}
void vault::stopAllKeysCreated(){if(sqlRequired(getMain()).getAllKeysCreated())return;sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).setStopNewKeys(true);std::this_thread::sleep_for(std::chrono::milliseconds(2000));}
void vault::passwordLockDB(const std::optional<std::string>& password){
    auto base=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getBaseSeed();
    if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("DB already locked!");
    MiniString phrase(requiredText(sqlRequired(base).getPhrase()));MiniData data(phrase.getData());MiniData encrypted;
    try{encrypted=PasswordCrypto::encryptPassword(requiredText(password),data);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){throw CommandException(nativeExceptionText(error));}
    sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).setEncryptedSeed(encrypted);sqlRequired(MinimaDB::getDB()).saveUserDB();
    try{sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).wipeBaseSeedRow();}catch(const SqlException& error){throw CommandException(sqlExceptionText(error));}
}
void vault::passowrdUnlockDB(const std::optional<std::string>& password){
    if(sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("DB already unlocked!");
    auto encrypted=sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).getEncryptedSeed();MiniData decrypted;
    try{decrypted=PasswordCrypto::decryptPassword(requiredText(password),encrypted);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){throw CommandException("Incorrect password!");}
    auto phrase=MiniString(decrypted.bytes()).toString();auto seed=BIP39::convertStringToSeed(phrase);
    if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).resetBaseSeedPrivKeys(phrase,seed.to0xString()))throw CommandException("Error updating Private keys.. please try again");
    sqlRequired(sqlRequired(MinimaDB::getDB()).getUserDB()).setEncryptedSeed(MiniData::ZERO_TXPOWID);sqlRequired(MinimaDB::getDB()).saveUserDB();
}
}
