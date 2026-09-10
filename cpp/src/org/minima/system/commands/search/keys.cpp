#include "keys.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/utils/BIP39.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <bit>
namespace minima::commands {
namespace {const std::string& required(const std::optional<std::string>& text){if(!text)throw JavaNullPointerException("null key string");return *text;}}
std::function<void(std::shared_ptr<Message>)> keys::postMessage=[](auto){throw JavaNullPointerException("null Main receiver");};
keys::keys():Command("keys","(action:list|new|checkkeys|genkey) (publickey:) - Get a list of all your public keys or create a new key"){}
std::optional<std::string> keys::getFullHelp() const {return std::string("\nkeys\n\nGet a list of all your public keys or create a new key.\n\nEach public key can be used for signing securely 262144 (64^3) times.\n\naction: (optional)\n    list : List your existing public keys. The default.\n    checkkeys : Checks if your Public and Private keys are correct.\n    new : Create a new key pair that is stored and managed in your DB.\n    genkey : Generate a new key NOT stored in the DB.\n\npublickey: (optional)\n    Search for a specific public key.\n\nExamples:\n\nkeys\n\nkeys action:list\n\nkeys action:genkey\n\nkeys action:checkkeys\n\nkeys action:list publickey:0xFFEE56..\n\nkeys action:new\n");}
std::vector<std::string> keys::getValidParams() const {return {"action","publickey","phrase","modifier","keyuses"};}
std::shared_ptr<Command> keys::getFunction() const {return std::make_shared<keys>();}
json::JSONValue keys::runCommand(){
    auto result=getJSONReply();auto wallet=sqlRequired(MinimaDB::getDB()).getWallet();auto action=getParam("action","list");
    if(action=="list"){
        auto publicKey=getParam("publickey","");bool searchKey=!publicKey.empty();if(searchKey)publicKey=MiniData::fromHex(publicKey).to0xString();
        auto modifier=getParam("modifier","");bool searchModifier=!modifier.empty();auto rows=sqlRequired(wallet).getAllKeys();json::JSONArray list;int32_t maximum=0;
        for(const auto& row:rows){auto details=sqlRequired(row).toJSON();
            if((searchKey||searchModifier)&&details.get(searchKey?"publickey":"modifier").isNull())throw JavaNullPointerException("null key filter receiver");
            bool match=searchKey?details.getString("publickey")==publicKey:searchModifier?details.getString("modifier")==modifier:true;
            if(match){if(row->getUses()>maximum)maximum=row->getUses();list.add(details);if(searchKey||searchModifier)break;}
        }
        json::JSONObject value;value.put("keys",list);value.put("total",int64_t(list.size()));value.put("maxuses",maximum);result.put("response",value);
    }else if(action=="genkey"){
        auto phrase=existsParam("phrase")?getParam("phrase"):BIP39::convertWordListToString(BIP39::getNewWordList());
        auto seed=BIP39::convertStringToSeed(phrase);MiniData modifier{BigInt(int64_t(0))};auto privateSeed=Crypto::getInstance().hashObjects(seed,modifier);auto tree=TreeKey::createDefault(privateSeed);
        if(!tree.getPublicKey()||!tree.getPrivateKey())throw JavaNullPointerException("null generated key");
        auto script="RETURN SIGNEDBY("+tree.getPublicKey()->to0xString()+")";Address address(script);json::JSONObject value;
        value.put("phrase",phrase);value.put("privatekey",tree.getPrivateKey()->to0xString());value.put("modifier",0);value.put("publickey",tree.getPublicKey()->to0xString());value.put("script",script);value.put("address",address.getAddressData().to0xString());value.put("miniaddress",Address::makeMinimaAddress(address.getAddressData()));result.put("response",value);
    }else if(action=="checkkeys"){
        if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).isBaseSeedAvailable())throw CommandException("Cannot check keys of locked DB..");
        auto rows=sqlRequired(wallet).getAllKeys();uint32_t correct=0,wrong=0;
        for(const auto& row:rows){auto& key=sqlRequired(row);TreeKey tree(MiniData::fromHex(required(key.getPrivateKey())),key.getSize(),key.getDepth());auto publicKey=MiniData::fromHex(required(key.getPublicKey()));auto actual=tree.getPublicKey();if(!actual)throw JavaNullPointerException("null generated key");
            if(!publicKey.isEqual(*actual)){MinimaLogger::log("[!] INCORRECT Public key : "+publicKey.to0xString()+" / "+actual->to0xString());++wrong;}
            else{MinimaLogger::log("CORRECT Public key : "+publicKey.to0xString());++correct;}
        }
        json::JSONObject value;value.put("allkeys",int64_t(rows.size()));value.put("correct",std::bit_cast<int32_t>(correct));value.put("wrong",std::bit_cast<int32_t>(wrong));result.put("response",value);
    }else if(action=="new"){
        auto row=sqlRequired(wallet).createNewKey();result.put("response",sqlRequired(row).toJSON());
    }else if(action=="createallkeys"){
        auto uses=getNumberParam("keyuses").getAsInt();auto message=std::make_shared<Message>("MAIN_LOAD_ALL_KEYS");message->addInteger("keyuses",uses);postMessage(message);
        json::JSONObject value;value.put("message","Keys being created..");result.put("response",value);
    }else throw CommandException("Unknown action : "+action);
    return result;
}
bool keys::checkKey(const std::optional<std::string>& publicText){
    auto publicKey=MiniData::fromHex(required(publicText));auto row=sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).getKeyFromPublic(publicText);
    auto& key=sqlRequired(row);if(!sqlRequired(sqlRequired(MinimaDB::getDB()).getWallet()).checkSingleKey(key.getPrivateKey(),key.getModifier()))return false;
    TreeKey tree(MiniData::fromHex(required(key.getPrivateKey())),key.getSize(),key.getDepth());auto actual=tree.getPublicKey();if(!actual)throw JavaNullPointerException("null generated key");return publicKey.isEqual(*actual);
}
}
