#include "SqlRecordingFixture.hpp"
#include "org/minima/database/wallet/Wallet.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
template<class T>JSONValue render(const std::shared_ptr<T>& row){return row?JSONValue(row->toJSON()):JSONValue();}
template<class T>JSONValue render(const std::vector<std::shared_ptr<T>>& rows){JSONArray array;for(auto& row:rows)array.add(render(row));return array;}
class DB:public Wallet {
public:
    DB():Wallet({[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();},[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}}){}
    void create(){createSQL();}
    void clear(){mSQLConnection.reset();}
    JSONObject caches(){JSONObject out;auto list=[](const auto& set){JSONArray a;for(const auto& s:set)a.add(s?JSONValue(*s):JSONValue());return a;};out.put("keys",list(mAllKeys));out.put("tracked",list(mAllTrackedAddress));out.put("simple",list(mAllSimpleAddress));return out;}
};
JSONObject run(const JSONObject& input,const std::string& root){
    connectionID=preparedID=0;GeneralParams::TEST_PARAMS=input.get("test").isNull()?false:input.get("test").asBool();Wallet::NUMBER_GETADDRESS_KEYS=64;DB db;JSONArray outputs;std::shared_ptr<SeedRow> held;std::vector<std::shared_ptr<SeedRow>> seeds;
    auto id=[&](const auto& p){if(!p)return -1;auto at=std::find(seeds.begin(),seeds.end(),p);if(at==seeds.end()){seeds.push_back(p);return int(seeds.size()-1);}return int(at-seeds.begin());};
    for(const auto& entry:array(input.get("ops")).elements()){
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=0;JSONObject out;
        GeneralParams::SEED_PHRASE=config->containsKey("phraseParam")?config->get("phraseParam").asString():"fixture seed";GeneralParams::ANYSEED_PHRASE=!config->containsKey("any")||flag("any");
        if(config->containsKey("target"))Wallet::NUMBER_GETADDRESS_KEYS=int32_t(integer(*config,"target"));
        try{
            auto cmd=config->get("cmd").asString();JSONValue result;auto field=[&](const std::string& key){return string(config->get(key));};
            if(cmd=="load")db.loadDB(std::filesystem::path(root+"/wallet"));
            else if(cmd=="construct"){GeneralParams::TEST_PARAMS=flag("test");DB another;}
            else if(cmd=="create")db.create();else if(cmd=="clearConnection")db.clear();else if(cmd=="hard")db.hardCloseDB();else if(cmd=="save")db.saveDB(flag("compact"));else if(cmd=="open")result=db.isOpen();
            else if(cmd=="base")result=render(db.getBaseSeed());else if(cmd=="available")result=db.isBaseSeedAvailable();else if(cmd=="hold")held=db.getBaseSeed();
            else if(cmd=="mutate"){auto row=flag("held")?held:db.getBaseSeed();sqlRequired(row).mPhrase=field("phrase");row->mSeed=field("seed");}
            else if(cmd=="updateSeed")db.updateSeedRow(field("phrase"),field("seed"));else if(cmd=="wipeSeed")db.wipeBaseSeedRow();else if(cmd=="reset")db.resetDB(field("phrase"));
            else if(cmd=="resetPrivate")result=db.resetBaseSeedPrivKeys(field("phrase"),field("seed"));else if(cmd=="checkPrivate")result=db.checkAllPrivateKeys();else if(cmd=="checkSingle")result=db.checkSingleKey(field("privatekey"),field("modifier"));
            else if(cmd=="stop")db.setStopNewKeys(flag("stop"));else if(cmd=="shutdown")db.shuttingDown();else if(cmd=="defaults")result=db.getDefaultKeysNumber();else if(cmd=="init")result=config->containsKey("log")?db.initDefaultKeys(int32_t(integer(*config,"max")),flag("log")):db.initDefaultKeys(int32_t(integer(*config,"max")));
            else if(cmd=="default")result=render(db.getDefaultAddress());else if(cmd=="newKey")result=render(db.createNewKey());else if(cmd=="newSimple")result=render(db.createNewSimpleAddress(flag("default")));
            else if(cmd=="addScript")result=render(db.addScript(field("script"),flag("simple"),flag("default"),field("publickey"),flag("track")));else if(cmd=="remove")db.removeScript(field("address"));else if(cmd=="script")result=render(db.getScriptFromAddress(field("address")));
            else if(cmd=="keyRelevant")result=db.isKeyRelevant(field("publickey"));else if(cmd=="addressRelevant")result=db.isAddressRelevant(field("address"));else if(cmd=="addressSimple")result=db.isAddressSimple(field("address"));
            else if(cmd=="keys")result=render(db.getAllKeys());else if(cmd=="key")result=render(db.getKeyFromPublic(field("publickey")));else if(cmd=="addresses")result=render(db.getAllAddresses());
            else if(cmd=="sign"){std::optional<MiniData> data;if(field("data"))data=MiniData::fromHex(*field("data"));auto sig=db.signData(field("publickey"),data?&*data:nullptr);if(sig)result=MiniData(sig->serialise()).to0xString();}
            else if(cmd=="uses")db.updateAllKeyUses(int32_t(integer(*config,"uses")));else if(cmd=="increment")db.updateIncrementAllKeyUses(int32_t(integer(*config,"uses")));else throw std::invalid_argument(cmd);
            out.put("value",result);
        }catch(const std::exception& error){out.put("exception",sqlExceptionText(error));}
        out.put("base",render(db.getBaseSeed()));out.put("held",render(held));out.put("baseID",id(db.getBaseSeed()));out.put("heldID",id(held));out.put("caches",db.caches());out.put("target",Wallet::NUMBER_GETADDRESS_KEYS.load());out.put("phraseParam",GeneralParams::SEED_PHRASE);out.put("events",events);out.put("logs",logs);outputs.add(out);
    }
    JSONObject out;out.put("ops",outputs);return out;
}
}
int main(int argc,char** argv){if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
