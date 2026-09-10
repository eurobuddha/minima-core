#pragma once
#include "org/minima/system/commands/backup/vault.hpp"
#include "org/minima/utils/encrypt/PasswordCrypto.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include <fstream>
namespace minima {
struct WalletVaultFixture {
    inline static json::JSONArray mainCalls;
    inline static bool enabled=false,shutDown=false,stopped=false;
    inline static std::string root;
    static void called(const std::string& name){mainCalls.add(name);if(sql_fixture::config->containsKey("vaultFail")&&sql_fixture::config->getString("vaultFail")==name)throw std::runtime_error("fixture Main failure");}
    struct Main:commands::vault::MainReceiver {
        bool getAllKeysCreated() override {called("getAllKeysCreated");return sql_fixture::flag("keysCreated");}
        int32_t getAllDefaultKeysSize() override {called("getAllDefaultKeysSize");return MinimaDB::getDB()->getWallet()->getDefaultKeysNumber();}
        void restoreReady() override {called("restoreReady");}
        void setHasShutDown() override {called("setHasShutDown");shutDown=true;}
        void stopMessageProcessor() override {called("stopMessageProcessor");stopped=true;}
    };
    static void reset(const json::JSONObject& input,const std::string& path){
        enabled=input.containsKey("vault")&&input.get("vault").asBool();mainCalls=json::JSONArray();shutDown=stopped=false;root=path;Wallet::NUMBER_GETADDRESS_KEYS=64;
        commands::vault::getMain=[](){return std::shared_ptr<commands::vault::MainReceiver>{};};
        if(enabled)for(const auto* name:{"archive.mv.db","cascade.db","chaintree.db","p2p.db"}){std::error_code e;std::filesystem::remove(std::filesystem::path(root)/"databases"/name,e);}
    }
    static void init(){commands::vault::getMain=[](){return std::make_shared<Main>();};}
    static void stores(){
        auto folder=MinimaDB::getDB()->getBaseDBFolder();MinimaDB::getDB()->getTxPoWDB()->getSQLDB()->loadDB(folder/"txpow");MinimaDB::getDB()->getArchive()->loadDB(folder/"archive");
        std::filesystem::create_directories(folder);for(const auto* name:{"archive.mv.db","cascade.db","chaintree.db","p2p.db"}){std::ofstream file(folder/name);file<<"fixture";}
    }
    static json::JSONObject snapshot(){
        auto wallet=MinimaDB::getDB()->getWallet();auto base=wallet->getBaseSeed();json::JSONObject value;
        value.put("base",base?json::JSONValue(base->toJSON()):json::JSONValue());value.put("calls",mainCalls);value.put("stopNewKeys",wallet->STOP_NEWKEYS.load());value.put("shutdown",shutDown);value.put("stopped",stopped);
        auto encrypted=MinimaDB::getDB()->getUserDB()->getEncryptedSeed();value.put("encrypted",encrypted.to0xString());
        if(!encrypted.isEqual(MiniData::ZERO_TXPOWID))try{auto password=sql_fixture::config->containsKey("observePassword")?sql_fixture::config->getString("observePassword"):"fixture pass";value.put("decrypted",MiniString(PasswordCrypto::decryptPassword(password,encrypted).bytes()).toString());}catch(const std::exception&){value.put("decrypted","DECRYPT_ERROR");}
        json::JSONArray files;for(const auto* name:{"archive.mv.db","cascade.db","chaintree.db","p2p.db"})files.add(std::filesystem::exists(std::filesystem::path(root)/"databases"/name));value.put("files",files);return value;
    }
    static json::JSONValue verify(){json::JSONArray valid;auto row=MinimaDB::getDB()->getCustomTxnDB()->getTransactionRow("test");for(const auto& signature:sqlRequired(row->getWitness()).getAllSignatures()){TreeKey key;key.setPublicKey(signature.getRootPublicKey());valid.add(key.verify(row->getTransaction()->getTransactionID(),signature));}return valid;}
};
}
