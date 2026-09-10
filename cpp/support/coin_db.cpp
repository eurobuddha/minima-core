#include "SqlRecordingFixture.hpp"
#include "CoinFixture.hpp"
#include "org/minima/database/txpowtree/CoinDB.hpp"
#include "support/JavaUnixFile.hpp"
#include <fstream>
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
SqlDBServices services() {
    return {[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}};
}
Coin fixtureCoin() {
    auto coin=support::coinFixture(int(integer(*config,"index")));
    if(flag("invalidHash"))coin.resetCoinID(MiniData(std::vector<uint8_t>(65)));
    return coin;
}
JSONValue renderCoin(const std::shared_ptr<Coin>& coin) {
    if(!coin)return {};JSONObject result;result.put("wire",MiniData(coin->serialise()).to0xString());result.put("json",coin->toJSON());return result;
}
JSONObject run(const JSONObject& input,const std::string& root) {
    connectionID=preparedID=0;JSONArray outputs;
    std::vector<std::shared_ptr<CoinDB>> identities;
    auto id=[&](const std::shared_ptr<CoinDB>& db){if(!db)return -1;auto at=std::find(identities.begin(),identities.end(),db);if(at==identities.end()){identities.push_back(db);return int(identities.size()-1);}return int(at-identities.begin());};
    std::shared_ptr<CoinDB> held;
    for(const auto& entry:array(input.get("ops")).elements()) {
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=0;JSONObject output;
        SqlDB::File file=config->containsKey("file")?(config->get("file").isNull()?SqlDB::File():SqlDB::File(root+"/files/"+config->get("file").asString())):SqlDB::File(root+"/files/db/coin");
        auto normalized=javaUnixFile(file);std::string old=normalized?javaUnixAbsoluteFile(*normalized)+".mv.db":"";
        id(CoinDB::getTxPoWTreeCoinDB());
        try {
            if(normalized&&!config->get("seed").isNull()){
                auto mode=config->get("seed").asString();std::filesystem::create_directories(std::filesystem::path(old).parent_path());
                if(mode=="file"){std::ofstream stream(old);stream<<"fixture";}
                else {std::filesystem::create_directories(old);if(mode=="nonempty"){std::ofstream stream(old+"/child");stream<<"fixture";}}
            }
            JSONValue result;auto cmd=config->get("cmd").asString();auto db=flag("held")?held:CoinDB::getTxPoWTreeCoinDB();
            if(cmd=="create")CoinDB::createCoinDB(file,services());
            else if(cmd=="hold")held=db;
            else if(cmd=="instance")result=id(db);
            else if(cmd=="make")result=renderCoin(std::make_shared<Coin>(fixtureCoin()));
            else if(cmd=="load")sqlRequired(db).loadDB(file);
            else if(cmd=="encrypted")sqlRequired(db).loadEncryptedSQLDB(file,string(config->get("password")));
            else if(cmd=="hard")sqlRequired(db).hardCloseDB();
            else if(cmd=="save")sqlRequired(db).saveDB(flag("compact"));
            else if(cmd=="open")result=sqlRequired(db).isOpen();
            else if(cmd=="reopen")sqlRequired(db).closeAndReopen();
            else if(cmd=="size")result=sqlRequired(db).getSize();
            else if(cmd=="clear")sqlRequired(db).clearOldCoins(integer(*config,"height"));
            else if(cmd=="get"){
                std::optional<MiniData> tree;if(!config->get("tree").isNull())tree=MiniData::fromHex(config->get("tree").asString());
                JSONArray coins;for(const auto& coin:sqlRequired(db).getAllCoins(tree?&*tree:nullptr))coins.add(renderCoin(coin));result=coins;
            } else if(cmd=="insert"){
                std::optional<MiniData> tree;if(!config->get("tree").isNull())tree=MiniData::fromHex(config->get("tree").asString());
                std::optional<Coin> coin;if(!flag("nullCoin"))coin=fixtureCoin();
                std::optional<MiniNumber> block;if(!config->get("block").isNull())block.emplace(config->get("block").asString());
                result=sqlRequired(db).insertCoin(tree?&*tree:nullptr,coin?&*coin:nullptr,block?&*block:nullptr);
            } else throw std::invalid_argument(cmd);
            output.put("value",result);
        }catch(const std::exception& error){output.put("exception",sqlExceptionText(error));}
        auto current=CoinDB::getTxPoWTreeCoinDB();output.put("instance",id(current));output.put("held",id(held));
        output.put("file",current&&current->getSQLFile()?JSONValue(current->getSQLFile()->string()):JSONValue());
        output.put("heldFile",held&&held->getSQLFile()?JSONValue(held->getSQLFile()->string()):JSONValue());
        std::error_code error;output.put("oldExists",!old.empty()&&old.find('\0')==std::string::npos&&std::filesystem::exists(old,error));
        output.put("events",events);output.put("logs",logs);outputs.add(output);
    }
    JSONObject result;result.put("ops",outputs);return result;
}
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}
}
