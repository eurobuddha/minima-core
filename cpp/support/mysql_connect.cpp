#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/utils/mysql/MySQLConnect.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <unicode/uloc.h>
#include <iostream>
using namespace minima;using namespace minima::json;using namespace minima::sql_fixture;
namespace minima {class MySQLConnectFixture {public:static void debug(MySQLConnect& db,bool enabled){db.DEBUG=enabled;}static void clear(MySQLConnect& db){db.mConnection.reset();}};}
namespace {
template<class T>JSONValue encoded(const std::shared_ptr<T>& value){return value?JSONValue(MiniData(value->serialise()).to0xString()):JSONValue();}
MySQLConnectServices services(){return {[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{event("connect",{url,user?JSONValue(*user):JSONValue(),password?JSONValue(*password):JSONValue()});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();},[](const auto& text){logs.add(text);},TimerMessage::currentTimeMillis};}
JSONObject run(const JSONObject& input){
    connectionID=preparedID=0;UErrorCode status=U_ZERO_ERROR;uloc_setDefault(input.getString("locale").c_str(),&status);if(U_FAILURE(status))throw std::runtime_error("fixture locale");
    auto get=[&](const char* key,const char* fallback){return input.containsKey(key)?string(input.get(key)):std::optional<std::string>(fallback);};
    MySQLConnect db(get("host","fixture:3306"),get("database","fixture"),get("user","fixture"),get("password","synthetic"),input.get("readOnly").isNull()?false:input.get("readOnly").asBool(),services());
    std::vector<std::shared_ptr<TxPoW>> pows;std::vector<std::shared_ptr<TxBlock>> blocks;std::vector<std::shared_ptr<Cascade>> cascades;JSONArray pw,bw,cw;
    for(int i=0;i<3;++i){auto pow=support::archiveFixtureTxPoW(i,i==2?1:0);pows.push_back(pow);blocks.push_back(std::make_shared<TxBlock>(*pow));pw.add(encoded(pow));bw.add(encoded(blocks.back()));}
    for(int count:{0,3}){auto cascade=std::make_shared<Cascade>();for(int i=0;i<count;++i)cascade->addToTip(*support::archiveFixtureTxPoW(i,0));cascades.push_back(cascade);cw.add(encoded(cascade));}
    JSONArray outputs;
    for(const auto& step:array(input.get("ops")).elements()){
        config=&object(step);events=JSONArray();logs=JSONArray();calls=0;JSONObject out;auto before=TimerMessage::currentTimeMillis();
        try{
            JSONValue value;auto cmd=config->getString("cmd");auto index=integer(*config,"index");bool null=flag("null");
            if(cmd=="init")db.init();else if(cmd=="shutdown")db.shutdown();else if(cmd=="wipe")db.wipeAll();else if(cmd=="wipeCoins")db.wipeCoinsDB();else if(cmd=="debug")MySQLConnectFixture::debug(db,flag("enabled"));else if(cmd=="clear")MySQLConnectFixture::clear(db);
            else if(cmd=="count")value=db.getCount();else if(cmd=="first")value=db.loadFirstBlock();else if(cmd=="last")value=db.loadLastBlock();else if(cmd=="maxCoin")value=db.getMaxCoinBlock();else if(cmd=="totalCoins")value=db.getTotalCoins();
            else if(cmd=="cascadeSave")value=db.saveCascade(null?nullptr:cascades.at(size_t(index)).get());else if(cmd=="cascadeLoad")value=encoded(db.loadCascade());
            else if(cmd=="blockSave"){auto block=null?nullptr:blocks.at(size_t(index));if(block&&flag("badHash"))block->getTxPoW().setSuperParent(0,MiniData(std::vector<uint8_t>(33)));value=db.saveBlock(block.get());}
            else if(cmd=="blockID")value=encoded(db.loadBlockFromID(string(config->get("id"))));else if(cmd=="blockNum")value=encoded(db.loadBlockFromNum(integer(*config,"number")));
            else if(cmd=="range"){auto text=string(config->get("start"));std::optional<MiniNumber> start;if(text)start.emplace(*text);JSONArray values;for(const auto& block:db.loadBlockRange(start?&*start:nullptr))values.add(encoded(block));value=values;}
            else if(cmd=="txpowSave"){auto pow=null?nullptr:pows.at(size_t(index));if(pow&&flag("badHash"))pow->setSuperParent(0,MiniData(std::vector<uint8_t>(33)));value=db.saveTxPoW(pow.get());}
            else if(cmd=="txpowGet")value=encoded(db.getTxPoW(string(config->get("id"))));
            else if(cmd=="coin"){
                auto coin=support::coinFixture(int(index));if(config->containsKey("entry"))coin.setMMREntryNumber(MMREntryNumber(BigInt(config->getString("entry")),int32_t(integer(*config,"scale"))));
                if(flag("noToken"))coin.setToken(std::nullopt);if(flag("nullState"))coin.setSharedState(nullptr);
                db.insertCoin(null?nullptr:&coin,integer(*config,"spentAt"),string(config->get("date")));
            }else if(cmd=="sql")value=db.searchCoins(string(config->get("sql")),flag("hideToken"));else throw std::invalid_argument(cmd);
            out.put("value",value);
        }catch(const std::exception& error){out.put("exception",sqlExceptionText(error));}
        out.put("before",before);out.put("after",TimerMessage::currentTimeMillis());out.put("events",events);out.put("logs",logs);outputs.add(out);
    }JSONObject result;result.put("pows",pw);result.put("blocks",bw);result.put("cascades",cw);result.put("ops",outputs);return result;
}
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
