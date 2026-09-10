#include "SqlTxPoWFixture.hpp"
#include "org/minima/database/txpowdb/TxPoWDB.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
std::vector<std::shared_ptr<TxPoW>> fixtures;
std::vector<std::shared_ptr<Message>> messages;
std::vector<std::shared_ptr<RamData>> ramObjects;
int clockCalls;
int index(const std::shared_ptr<TxPoW>& txpow){if(!txpow)return -2;for(size_t i=0;i<fixtures.size();++i)if(fixtures[i]==txpow)return int(i);return -1;}
void service(const std::string& name,std::initializer_list<JSONValue> args={}){
    try{event(name,args);}catch(const SqlException& error){throw SqlRuntimeException("java.lang.IllegalStateException",error.what());}
    if(!config->get("serviceFail").isNull()&&config->get("serviceFail").asString()==name)throw SqlRuntimeException("java.lang.IllegalStateException","service failure");
}
bool contains(const std::string& key,const std::string& value){if(config->get(key).isNull())return false;for(const auto& item:array(config->get(key)).elements())if(!item.isNull()&&item.asString()==value)return true;return false;}
class DB:public TxPoWDB {
public:
    DB():TxPoWDB({{[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user,password});return std::make_shared<Connection>();},[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}},
        [](auto message){if(flag("nullMain"))throw SqlRuntimeException("java.lang.NullPointerException","null Main");auto txpow=message->getObject("txpow").template as<std::shared_ptr<TxPoW>>();service("main",{message->getMessageType().value(),index(txpow),txpow->getTxPoWID()});messages.push_back(message);},
        []{if(flag("nullDB"))throw SqlRuntimeException("java.lang.NullPointerException","null MinimaDB");service("wallet");bool missing=flag("nullWallet");ChainServices wallet;
            wallet.isAddressRelevant=[missing](const std::string& id){if(missing)throw SqlRuntimeException("java.lang.NullPointerException","null Wallet");service("address",{id});return contains("addresses",id);};
            wallet.isKeyRelevant=[missing](const std::string& id){if(missing)throw SqlRuntimeException("java.lang.NullPointerException","null Wallet");service("key",{id});return contains("keys",id);};return wallet;}},[]{++clockCalls;return integer(*config,"now",1700000000000LL);}){}
    void retention(int64_t time){mRamDB->MAX_TIME=time;}
};
JSONValue pow(const std::shared_ptr<TxPoW>& txpow){if(!txpow)return {};JSONObject result;result.put("index",index(txpow));result.put("id",txpow->getTxPoWID());return result;}
JSONArray pows(const TxPoWDB::TxPoWs& txpows){JSONArray result;for(const auto& txpow:txpows)result.add(pow(txpow));return result;}
JSONValue pool(const std::shared_ptr<RamDB::Map>& map){
    if(!map)return {};JSONArray result;
    for(const auto& key:map->keys()){
        auto data=map->get(key).value();auto found=std::find(ramObjects.begin(),ramObjects.end(),data);
        int identity=int(found-ramObjects.begin());if(found==ramObjects.end())ramObjects.push_back(data);
        JSONObject row;row.put("key",key);row.put("object",identity);row.put("txpow",pow(data->getTxPoW()));row.put("access",data->getLastAccess());row.put("chain",data->isOnMainChain());row.put("cascade",data->isInCascade());result.add(row);
    }return result;
}
std::shared_ptr<TxPoW> selected(){return config->get("index").isNull()?nullptr:fixtures.at(size_t(integer(*config,"index")));}
std::optional<std::string> selectedID(){if(config->containsKey("id"))return string(config->get("id"));auto txpow=selected();return txpow?std::optional(txpow->getTxPoWID()):std::nullopt;}
JSONObject run(const JSONObject& input,const std::string& root){
    fixtures.clear();messages.clear();ramObjects.clear();connectionID=preparedID=0;
    for(const auto& spec:array(input.get("fixtures")).elements()){config=&object(spec);fixtures.push_back(fixture());}
    DB db;std::shared_ptr<RamDB::Map> held;JSONArray outputs,encoded;
    for(const auto& txpow:fixtures)encoded.add(render(txpow));
    for(const auto& entry:array(input.get("ops")).elements()){
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=clockCalls=0;JSONObject output;
        int64_t before=TimerMessage::currentTimeMillis();
        try{
            JSONValue result;auto cmd=config->get("cmd").asString();
            if(cmd=="load")db.loadSQLDB(config->containsKey("file")?(config->get("file").isNull()?SqlDB::File():SqlDB::File(config->get("file").asString())):SqlDB::File(root+"/db/txpow"));
            else if(cmd=="save")db.saveDB(flag("compact"));
            else if(cmd=="hard")db.hardCloseSQLDB();
            else if(cmd=="add")result=db.addTxPoW(selected());
            else if(cmd=="addSQL")db.addSQLTxPoW(selected());
            else if(cmd=="get")result=pow(db.getTxPoW(selectedID()));
            else if(cmd=="exists")result=db.exists(selectedID());
            else if(cmd=="all"){
                if(config->get("ids").isNull())result=pows(db.getAllTxPoW(nullptr));
                else{TxPoWDB::IDs ids;for(const auto& id:array(config->get("ids")).elements())ids.push_back(string(id));result=pows(db.getAllTxPoW(ids));}
            }
            else if(cmd=="children")result=pows(db.getChildBlocks(selectedID()));
            else if(cmd=="ramSize")result=db.getRamSize();
            else if(cmd=="sqlSize")result=db.getSqlSize();
            else if(cmd=="file")result=db.getSqlFile()?JSONValue(db.getSqlFile()->string()):JSONValue();
            else if(cmd=="cleanRAM"){db.retention(integer(*config,"retention",3600000));db.cleanDBRAM();}
            else if(cmd=="wipeRAM")db.wipeDBRAM();
            else if(cmd=="cleanSQL")db.cleanDBSQL();
            else if(cmd=="clearChain")db.clearMainChainTxns();
            else if(cmd=="chain")db.setOnMainChain(selectedID());
            else if(cmd=="cascade")db.setInCascade(selectedID());
            else if(cmd=="unused")result=pows(db.getAllUnusedTxns());
            else if(cmd=="remove")db.removeMemPoolTxPoW(selectedID());
            else if(cmd=="coin")result=db.checkMempoolCoins(MiniData::fromHex(config->get("coinid").asString()));
            else if(cmd=="hold")held=db.getCompleteMemPool();
            else if(cmd=="mutate"){auto txpow=selected();txpow->setNonce(MiniNumber(integer(*config,"nonce")));txpow->calculateTXPOWID();}
            else throw std::invalid_argument(cmd);
            output.put("value",result);
        }catch(const std::exception& error){output.put("exception",sqlExceptionText(error));}
        auto current=db.getCompleteMemPool();output.put("pool",pool(current));output.put("held",pool(held));output.put("samePool",current==held);
        JSONArray posted;for(const auto& message:messages){JSONObject row;row.put("type",message->getMessageType().value());row.put("txpow",pow(message->getObject("txpow").as<std::shared_ptr<TxPoW>>()));posted.add(row);}output.put("posted",posted);
        output.put("before",before);output.put("after",TimerMessage::currentTimeMillis());output.put("sqlNow",integer(*config,"now",1700000000000LL));output.put("clockCalls",clockCalls);
        output.put("events",events);output.put("logs",logs);output.put("sqlFile",db.getSqlFile()?JSONValue(db.getSqlFile()->string()):JSONValue());output.put("chainFile",db.getOnChainDB()->getSQLFile()?JSONValue(db.getOnChainDB()->getSQLFile()->string()):JSONValue());outputs.add(output);
    }
    JSONObject result;result.put("fixtures",encoded);result.put("ops",outputs);return result;
}
}
int main(int argc,char** argv){if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
