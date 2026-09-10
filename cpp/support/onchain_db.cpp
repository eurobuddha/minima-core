#include "SqlRecordingFixture.hpp"
#include "org/minima/database/txpowdb/onchain/TxPoWOnChainDB.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
int clockCalls;
const int64_t defaultRetention=TxPoWOnChainDB::MAX_ONCHAINSQL_MILLI.load();
class DB:public TxPoWOnChainDB {
public:
    DB():TxPoWOnChainDB({[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}},[]{++clockCalls;return integer(*config,"now",1700000000000LL);}){}
    void create(){createSQL();}
    void clear(){mSQLConnection.reset();}
};
JSONObject run(const JSONObject& input,const std::string& root) {
    connectionID=preparedID=0;TxPoWOnChainDB::MAX_ONCHAINSQL_MILLI=defaultRetention;DB db;JSONArray outputs;
    for(const auto& entry:array(input.get("ops")).elements()) {
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=clockCalls=0;JSONObject output;
        if(config->containsKey("retention"))TxPoWOnChainDB::MAX_ONCHAINSQL_MILLI=integer(*config,"retention");
        try {
            JSONValue result;auto cmd=config->get("cmd").asString();
            if(cmd=="load")db.loadDB(std::filesystem::path(root+"/db/onchain"));
            else if(cmd=="create")db.create();
            else if(cmd=="wipe")db.wipeDB();
            else if(cmd=="clear")db.clear();
            else if(cmd=="hard")db.hardCloseDB();
            else if(cmd=="save")db.saveDB(flag("compact"));
            else if(cmd=="open")result=db.isOpen();
            else if(cmd=="add"){
                std::optional<MiniNumber> block;
                if(!config->get("block").isNull())block.emplace(config->get("block").asString());
                result=db.addOnChainTxPoW(string(config->get("blockid")),block?&*block:nullptr,string(config->get("txpowid")));
            }
            else if(cmd=="get")result=db.getOnChainTxPoW(string(config->get("txpowid")));
            else if(cmd=="inblock")result=db.getInBlockTxPoW(string(config->get("blockid")));
            else if(cmd=="inheight")result=db.getInBlockTxPoW(integer(*config,"height"));
            else if(cmd=="first")result=db.getFirstTxPoW();
            else if(cmd=="last")result=db.getLastTxPoW();
            else if(cmd=="size")result=db.getSize();
            else if(cmd=="clean")result=config->containsKey("hard")?db.cleanDB(flag("hard")):db.cleanDB();
            else throw std::invalid_argument(cmd);
            output.put("value",result);
        }catch(const std::exception& error){output.put("exception",sqlExceptionText(error));}
        output.put("before",integer(*config,"now",1700000000000LL));output.put("after",integer(*config,"now",1700000000000LL));output.put("clockCalls",clockCalls);
        output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()?JSONValue(db.getSQLFile()->string()):JSONValue());output.put("retention",TxPoWOnChainDB::MAX_ONCHAINSQL_MILLI.load());outputs.add(output);
    }
    JSONObject result;result.put("ops",outputs);result.put("defaultRetention",defaultRetention);return result;
}
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try {std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}
}
