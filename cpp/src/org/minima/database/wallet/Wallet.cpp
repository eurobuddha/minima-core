#include "Wallet.hpp"
#include "org/minima/objects/Address.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/JavaRandom48.hpp"
#include "support/MultisigContract.hpp"
#include <bit>
namespace minima {
namespace {
const std::string& required(const Wallet::Text& text){if(!text)throw JavaNullPointerException("null wallet string");return *text;}
int32_t wrap(int64_t value){return std::bit_cast<int32_t>(uint32_t(value));}
}
Wallet::Wallet(SqlDBServices services):SqlDB(std::move(services)){if(GeneralParams::TEST_PARAMS)NUMBER_GETADDRESS_KEYS=8;}
void Wallet::shuttingDown(){mShuttingdown=true;}
bool Wallet::isShuttingDown(){return mShuttingdown;}
void Wallet::createSQL(){
    auto statement=sqlRequired(mSQLConnection).createStatement();
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `keys` (""  `id` bigint auto_increment,""  `size` int NOT NULL,""  `depth` int NOT NULL,""  `uses` bigint NOT NULL,""  `maxuses` bigint NOT NULL,""  `modifier` varchar(80) NOT NULL,""  `privatekey` varchar(80) NOT NULL,""  `publickey` varchar(80) NOT NULL"")");
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `scripts` (""  `id` bigint auto_increment,""  `script` varchar(8192) NOT NULL,""  `address` varchar(80) NOT NULL,""  `simple` int NOT NULL,""  `defaultaddress` int NOT NULL,""  `publickey` varchar(80) NOT NULL,""  `track` int NOT NULL"")");
    sqlRequired(statement).execute("CREATE TABLE IF NOT EXISTS `seed` (""  `id` int NOT NULL UNIQUE,""  `phrase` varchar(8192) NOT NULL,""  `seed` varchar(80) NOT NULL"")");
    sqlRequired(statement).close();
    SQL_CREATE_PUBLIC_KEY=sqlRequired(mSQLConnection).prepareStatement("INSERT IGNORE INTO keys ( size, depth, uses, maxuses, modifier, privatekey, publickey ) VALUES ( ?, ?, ?, ? ,? ,? ,? )");
    SQL_GET_ALL_KEYS=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM keys");
    SQL_GET_KEY=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM keys WHERE publickey=?");
    SQL_UPDATE_KEY_USES=sqlRequired(mSQLConnection).prepareStatement("UPDATE keys SET uses=? WHERE publickey=?");
    SQL_UPDATE_ALL_KEY_USES=sqlRequired(mSQLConnection).prepareStatement("UPDATE keys SET uses=?");
    SQL_UPDATE_INC_ALL_KEY_USES=sqlRequired(mSQLConnection).prepareStatement("UPDATE keys SET uses=uses+?");
    SQL_WIPE_PRIVATE_KEYS=sqlRequired(mSQLConnection).prepareStatement("UPDATE keys SET privatekey='0x00' WHERE privatekey!='0x00'");
    SQL_UPDATE_PRIVATE_KEYS=sqlRequired(mSQLConnection).prepareStatement("UPDATE keys SET privatekey=? WHERE publickey=?");
    SQL_ADD_SCRIPT=sqlRequired(mSQLConnection).prepareStatement("INSERT IGNORE INTO scripts ( script, address, simple, defaultaddress, publickey, track ) VALUES ( ? , ? , ? , ? , ? , ? )");
    SQL_REMOVE_SCRIPT=sqlRequired(mSQLConnection).prepareStatement("DELETE FROM scripts WHERE address=?");
    SQL_LIST_ALL_SCRIPTS=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM scripts");
    SQL_LIST_SIMPLE_SCRIPTS=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM scripts WHERE simple<>0");
    SQL_LIST_TRACK_SCRIPTS=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM scripts WHERE track<>0");
    SQL_LIST_DEFAULT_SCRIPTS=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM scripts WHERE defaultaddress<>0");
    SQL_GET_SCRIPT=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM scripts WHERE address=?");
    SQL_SELECT_SEED=sqlRequired(mSQLConnection).prepareStatement("SELECT * FROM seed WHERE id=1");
    SQL_INSERT_SEED=sqlRequired(mSQLConnection).prepareStatement("INSERT INTO seed ( id, phrase, seed ) VALUES ( 1 , ? , ? )");
    SQL_UPDATE_SEED=sqlRequired(mSQLConnection).prepareStatement("UPDATE seed SET phrase=?, seed=? WHERE id=1");
    mAllKeys.clear();mAllTrackedAddress.clear();mAllSimpleAddress.clear();
    for(const auto& key:getAllKeys())mAllKeys.insert(sqlRequired(key).getPublicKey());
    for(const auto& row:getAllAddresses()){auto address=sqlRequired(row).getAddress();if(row->isTrack())mAllTrackedAddress.insert(address);if(row->isSimple())mAllSimpleAddress.insert(address);}
    initBaseSeed();
    auto address=Address(MULTISIG_CONTRACT).getAddressData().to0xString();
    if(!getScriptFromAddress(address)){log("Adding base MULTISIG address "+address);addScript(MULTISIG_CONTRACT,false,false,"0x00",false);}
}
void Wallet::resetDB(const Text& phrase){
    auto stmt=sqlRequired(mSQLConnection).createStatement();sqlRequired(stmt).execute("DROP ALL OBJECTS");sqlRequired(stmt).close();sqlRequired(mSQLConnection).close();mSQLConnection.reset();checkOpen();
    auto seed=BIP39::convertStringToSeed(required(phrase));updateSeedRow(phrase,seed.to0xString());
}
void Wallet::initBaseSeed(){
    auto rows=sqlRequired(SQL_SELECT_SEED).executeQuery();
    if(sqlRequired(rows).next()){mBaseSeed=std::make_shared<SeedRow>(*rows);return;}
    log("Generating Base Private Seed Key");auto words=BIP39::getNewWordList();std::string phrase;
    if(!GeneralParams::SEED_PHRASE.empty()){
        log(std::string("Using provided seed phrase from params.. anyseed:")+(GeneralParams::ANYSEED_PHRASE?"true":"false"));
        phrase=GeneralParams::ANYSEED_PHRASE?javaStringTrim(GeneralParams::SEED_PHRASE):BIP39::cleanSeedPhrase(javaStringTrim(GeneralParams::SEED_PHRASE));GeneralParams::SEED_PHRASE="";
    }else phrase=BIP39::convertWordListToString(words);
    auto seed=BIP39::convertStringToSeed(phrase);sqlRequired(SQL_INSERT_SEED).clearParameters();sqlRequired(SQL_INSERT_SEED).setString(1,phrase);sqlRequired(SQL_INSERT_SEED).setString(2,seed.to0xString());sqlRequired(SQL_INSERT_SEED).execute();mBaseSeed=std::make_shared<SeedRow>(phrase,seed.to0xString());
}
void Wallet::updateSeedRow(const Text& phrase,const Text& seed){sqlRequired(SQL_UPDATE_SEED).clearParameters();sqlRequired(SQL_UPDATE_SEED).setString(1,phrase);sqlRequired(SQL_UPDATE_SEED).setString(2,seed);sqlRequired(SQL_UPDATE_SEED).executeUpdate();mBaseSeed=std::make_shared<SeedRow>(phrase,seed);}
std::shared_ptr<SeedRow> Wallet::getBaseSeed(){std::lock_guard lock(mSQLLock);return mBaseSeed;}
bool Wallet::isBaseSeedAvailable(){std::lock_guard lock(mSQLLock);return required(sqlRequired(mBaseSeed).getSeed())!="0x00";}
void Wallet::wipeBaseSeedRow(){
    std::lock_guard lock(mSQLLock);sqlRequired(SQL_WIPE_PRIVATE_KEYS).execute();sqlRequired(SQL_UPDATE_SEED).clearParameters();sqlRequired(SQL_UPDATE_SEED).setString(1,"");sqlRequired(SQL_UPDATE_SEED).setString(2,"0x00");sqlRequired(SQL_UPDATE_SEED).executeUpdate();mBaseSeed=std::make_shared<SeedRow>("","0x00");saveDB(true);checkOpen();
}
bool Wallet::resetBaseSeedPrivKeys(const Text& phrase,const Text& seed){
    try{updateSeedRow(phrase,seed);auto keys=getAllKeys();auto base=MiniData::fromHex(required(seed));
        for(const auto& key:keys){auto modifier=MiniData::fromHex(required(sqlRequired(key).getModifier()));auto priv=Crypto::getInstance().hashObjects(base,modifier);sqlRequired(SQL_UPDATE_PRIVATE_KEYS).clearParameters();sqlRequired(SQL_UPDATE_PRIVATE_KEYS).setString(1,priv.to0xString());sqlRequired(SQL_UPDATE_PRIVATE_KEYS).setString(2,key->getPublicKey());sqlRequired(SQL_UPDATE_PRIVATE_KEYS).execute();}
    }catch(const SqlException& error){log(sqlExceptionText(error));return false;}return true;
}
bool Wallet::checkAllPrivateKeys(){bool ok=true;for(const auto& key:getAllKeys())if(!checkSingleKey(sqlRequired(key).getPrivateKey(),key->getModifier())){log("[SERIOUS ERROR] Private key NOT == Seed+Modifier publickey:"+key->getPublicKey().value_or("null"));ok=false;}return ok;}
bool Wallet::checkSingleKey(const Text& privateKey,const Text& modifier){auto priv=MiniData::fromHex(required(privateKey));auto mod=MiniData::fromHex(required(modifier));auto base=MiniData::fromHex(required(sqlRequired(mBaseSeed).getSeed()));return Crypto::getInstance().hashObjects(base,mod).isEqual(priv);}
void Wallet::setStopNewKeys(bool stop){STOP_NEWKEYS=stop;log(STOP_NEWKEYS?"Disallow creation of NEW default keys..":"Re-allow creation of NEW default keys..");}
int32_t Wallet::getDefaultKeysNumber(){return int32_t(getAllDefaultAddresses().size());}
bool Wallet::initDefaultKeys(int32_t max){return initDefaultKeys(max,false);}
bool Wallet::initDefaultKeys(int32_t max,bool logs){
    if(isShuttingDown()||STOP_NEWKEYS)return true;auto scripts=getAllDefaultAddresses();bool all=false;int32_t count=int32_t(scripts.size());
    if(count<NUMBER_GETADDRESS_KEYS){int32_t diff=wrap(int64_t(NUMBER_GETADDRESS_KEYS.load())-count);if(diff>max)diff=max;
        for(int32_t i=0;i<diff;++i)if(!isShuttingDown()&&!STOP_NEWKEYS){createNewSimpleAddress(true);if(logs)log("Default key created.. total:"+std::to_string(wrap(int64_t(count)+i+1)));}
        log(std::to_string(diff)+" more initial keys created.. Total now : "+std::to_string(wrap(int64_t(count)+diff))+" / "+std::to_string(NUMBER_GETADDRESS_KEYS.load()));
    }else all=true;
    if(int32_t(getAllDefaultAddresses().size())>=NUMBER_GETADDRESS_KEYS)return true;return all;
}
std::shared_ptr<ScriptRow> Wallet::getDefaultAddress(){auto scripts=getAllDefaultAddresses();auto index=JavaRandom48().nextInt(int32_t(scripts.size()));return scripts.at(size_t(index));}
std::shared_ptr<ScriptRow> Wallet::createNewSimpleAddress(bool isDefault){auto key=createNewKey();auto script="RETURN SIGNEDBY("+sqlRequired(key).getPublicKey().value_or("null")+")";return addScript(script,true,isDefault,key->getPublicKey(),true);}
std::shared_ptr<KeyRow> Wallet::createNewKey(){
    std::lock_guard lock(mSQLLock);if(!isBaseSeedAvailable())throw std::invalid_argument("KeysDB LOCKED. No More Keys Allowed. Base SEED missing..");
    MiniData modifier(BigInt(int64_t(mAllKeys.size())));auto priv=Crypto::getInstance().hashObjects(MiniData::fromHex(required(sqlRequired(mBaseSeed).getSeed())),modifier);auto tree=TreeKey::createDefault(priv);
    try{sqlRequired(SQL_CREATE_PUBLIC_KEY).clearParameters();sqlRequired(SQL_CREATE_PUBLIC_KEY).setInt(1,tree.getSize());sqlRequired(SQL_CREATE_PUBLIC_KEY).setInt(2,tree.getDepth());sqlRequired(SQL_CREATE_PUBLIC_KEY).setInt(3,tree.getUses());sqlRequired(SQL_CREATE_PUBLIC_KEY).setInt(4,tree.getMaxUses());sqlRequired(SQL_CREATE_PUBLIC_KEY).setString(5,modifier.to0xString());sqlRequired(SQL_CREATE_PUBLIC_KEY).setString(6,tree.getPrivateKey()->to0xString());sqlRequired(SQL_CREATE_PUBLIC_KEY).setString(7,tree.getPublicKey()->to0xString());sqlRequired(SQL_CREATE_PUBLIC_KEY).execute();mAllKeys.insert(tree.getPublicKey()->to0xString());return std::make_shared<KeyRow>(tree.getSize(),tree.getDepth(),tree.getUses(),tree.getMaxUses(),"0x00",tree.getPrivateKey()->to0xString(),tree.getPublicKey()->to0xString());
    }catch(const SqlException& error){log(sqlExceptionText(error));}return {};
}
std::shared_ptr<ScriptRow> Wallet::addScript(const Text& script,bool simple,bool isDefault,const Text& publicKey,bool track){
    std::lock_guard lock(mSQLLock);auto address=Address(required(script)).getAddressData().to0xString();
    try{sqlRequired(SQL_ADD_SCRIPT).clearParameters();sqlRequired(SQL_ADD_SCRIPT).setString(1,script);sqlRequired(SQL_ADD_SCRIPT).setString(2,address);sqlRequired(SQL_ADD_SCRIPT).setInt(3,simple?1:0);sqlRequired(SQL_ADD_SCRIPT).setInt(4,isDefault?1:0);sqlRequired(SQL_ADD_SCRIPT).setString(5,publicKey);sqlRequired(SQL_ADD_SCRIPT).setInt(6,track?1:0);sqlRequired(SQL_ADD_SCRIPT).execute();if(track)mAllTrackedAddress.insert(address);if(simple)mAllSimpleAddress.insert(address);return std::make_shared<ScriptRow>(script,address,simple,isDefault,publicKey,track);
    }catch(const SqlException& error){log(sqlExceptionText(error));}return {};
}
void Wallet::removeScript(const Text& address){std::lock_guard lock(mSQLLock);try{sqlRequired(SQL_REMOVE_SCRIPT).clearParameters();sqlRequired(SQL_REMOVE_SCRIPT).setString(1,address);sqlRequired(SQL_REMOVE_SCRIPT).execute();}catch(const SqlException& error){log(sqlExceptionText(error));}}
std::shared_ptr<ScriptRow> Wallet::getScriptFromAddress(const Text& address){std::lock_guard lock(mSQLLock);try{sqlRequired(SQL_GET_SCRIPT).clearParameters();sqlRequired(SQL_GET_SCRIPT).setString(1,address);auto rows=sqlRequired(SQL_GET_SCRIPT).executeQuery();if(sqlRequired(rows).next())return std::make_shared<ScriptRow>(*rows);}catch(const SqlException& error){log(sqlExceptionText(error));}return {};}
bool Wallet::isKeyRelevant(const Text& key){std::lock_guard lock(mSQLLock);return mAllKeys.contains(key);}
bool Wallet::isAddressRelevant(const Text& address){std::lock_guard lock(mSQLLock);return mAllTrackedAddress.contains(address);}
bool Wallet::isAddressSimple(const Text& address){std::lock_guard lock(mSQLLock);return mAllSimpleAddress.contains(address);}
Wallet::Keys Wallet::getAllKeys(){std::lock_guard lock(mSQLLock);Keys result;try{auto rows=sqlRequired(SQL_GET_ALL_KEYS).executeQuery();while(sqlRequired(rows).next())result.push_back(std::make_shared<KeyRow>(*rows));}catch(const SqlException& error){log(sqlExceptionText(error));}return result;}
std::shared_ptr<KeyRow> Wallet::getKeyFromPublic(const Text& key){std::lock_guard lock(mSQLLock);try{sqlRequired(SQL_GET_KEY).clearParameters();sqlRequired(SQL_GET_KEY).setString(1,key);auto rows=sqlRequired(SQL_GET_KEY).executeQuery();if(sqlRequired(rows).next())return std::make_shared<KeyRow>(*rows);}catch(const SqlException& error){log(sqlExceptionText(error));}return {};}
Wallet::Scripts Wallet::getAllAddresses(){std::lock_guard lock(mSQLLock);Scripts result;try{auto rows=sqlRequired(SQL_LIST_ALL_SCRIPTS).executeQuery();while(sqlRequired(rows).next())result.push_back(std::make_shared<ScriptRow>(*rows));}catch(const SqlException& error){log(sqlExceptionText(error));}return result;}
Wallet::Scripts Wallet::getAllDefaultAddresses(){std::lock_guard lock(mSQLLock);Scripts result;try{auto rows=sqlRequired(SQL_LIST_DEFAULT_SCRIPTS).executeQuery();while(sqlRequired(rows).next())result.push_back(std::make_shared<ScriptRow>(*rows));}catch(const std::exception& error){log(sqlExceptionText(error));}return result;}
std::shared_ptr<Signature> Wallet::signData(const Text& publicKey,const MiniData* data){
    std::lock_guard lock(mSQLLock);if(required(sqlRequired(mBaseSeed).getSeed())=="0x00")throw std::invalid_argument("KeysDB LOCKED. No Private Keys..");
    try{sqlRequired(SQL_GET_KEY).clearParameters();sqlRequired(SQL_GET_KEY).setString(1,publicKey);auto rows=sqlRequired(SQL_GET_KEY).executeQuery();if(!sqlRequired(rows).next())return {};auto key=std::make_shared<KeyRow>(*rows);TreeKey tree(MiniData::fromHex(required(key->getPrivateKey())),key->getSize(),key->getDepth());tree.setLogger([this](const auto& text){log(text);});tree.setUses(key->getUses());auto signature=tree.sign(data);updateUses(tree.getUses(),key->getPublicKey());return std::make_shared<Signature>(std::move(signature));
    }catch(const SqlException& error){log(sqlExceptionText(error));}return {};
}
void Wallet::updateUses(int32_t uses,const Text& key){sqlRequired(SQL_UPDATE_KEY_USES).clearParameters();sqlRequired(SQL_UPDATE_KEY_USES).setInt(1,uses);sqlRequired(SQL_UPDATE_KEY_USES).setString(2,key);sqlRequired(SQL_UPDATE_KEY_USES).execute();}
void Wallet::updateAllKeyUses(int32_t uses){sqlRequired(SQL_UPDATE_ALL_KEY_USES).clearParameters();sqlRequired(SQL_UPDATE_ALL_KEY_USES).setInt(1,uses);sqlRequired(SQL_UPDATE_ALL_KEY_USES).execute();}
void Wallet::updateIncrementAllKeyUses(int32_t uses){sqlRequired(SQL_UPDATE_INC_ALL_KEY_USES).clearParameters();sqlRequired(SQL_UPDATE_INC_ALL_KEY_USES).setInt(1,uses);sqlRequired(SQL_UPDATE_INC_ALL_KEY_USES).execute();}
}
