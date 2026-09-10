#include "SqlRecordingFixture.hpp"
#include "org/minima/database/txpowdb/sql/TxPoWSqlDB.hpp"
#include "SqlTxPoWFixture.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
int clockCalls;
const int64_t defaultRetention=TxPoWSqlDB::maxSQLMilli().load();
class DB:public TxPoWSqlDB {
public:
    DB():TxPoWSqlDB({[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}},[]{++clockCalls;return integer(*config,"now",1700000000000LL);}){}
    void create(){createSQL();}
    void clear(){mSQLConnection.reset();}
};
JSONObject run(const JSONObject& input,const std::string& root) {
    connectionID=preparedID=0;TxPoWSqlDB::maxSQLMilli()=defaultRetention;TxPoWSqlDB::LOG_ADD_BLOCKS=false;TxPoWSqlDB::LOG_ADD_TRANSACTION=false;DB db;JSONArray outputs;
    for(const auto& entry:array(input.get("ops")).elements()) {
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=clockCalls=0;JSONObject output;
        if(config->containsKey("retention"))TxPoWSqlDB::maxSQLMilli()=integer(*config,"retention");
        TxPoWSqlDB::LOG_ADD_BLOCKS=flag("logBlocks");TxPoWSqlDB::LOG_ADD_TRANSACTION=flag("logTransactions");
        try {
            JSONValue result;auto cmd=config->get("cmd").asString();
            if(cmd=="load")db.loadDB(std::filesystem::path(root+"/db/txpow"));
            else if(cmd=="create")db.create();
            else if(cmd=="wipe")db.wipeDB();
            else if(cmd=="clear")db.clear();
            else if(cmd=="hard")db.hardCloseDB();
            else if(cmd=="save")db.saveDB(flag("compact"));
            else if(cmd=="open")result=db.isOpen();
            else if(cmd=="make")result=MiniData(fixture()->serialise()).to0xString();
            else if(cmd=="add"){
                auto txpow=flag("nullTxPoW")?nullptr:fixture();
                result=db.addTxPoW(txpow.get(),flag("relevant"));
            }
            else if(cmd=="get")result=render(db.getTxPoW(string(config->get("txpowid"))));
            else if(cmd=="children"){JSONArray list;for(const auto& id:db.getChildBlocks(string(config->get("parentid"))))list.add(id?JSONValue(*id):JSONValue());result=list;}
            else if(cmd=="relevant")result=render(config->containsKey("offset")?db.getAllRelevant(int32_t(integer(*config,"limit")),int32_t(integer(*config,"offset"))):db.getAllRelevant(int32_t(integer(*config,"limit"))));
            else if(cmd=="latest")result=render(db.getLatestTxPoW(int32_t(integer(*config,"limit")),int32_t(integer(*config,"offset"))));
            else if(cmd=="latestSize")result=db.getLatestTxPoWSize(integer(*config,"minTime"));
            else if(cmd=="relevantSize")result=db.getRelevantSize();
            else if(cmd=="size")result=db.getSize();
            else if(cmd=="custom")result=db.customSizeQuery(string(config->get("where")));
            else if(cmd=="exists")result=db.exists(string(config->get("txpowid")));
            else if(cmd=="reopen")db.closeAndReopen();
            else if(cmd=="clean")result=config->containsKey("hard")?db.cleanDB(flag("hard")):db.cleanDB();
            else throw std::invalid_argument(cmd);
            output.put("value",result);
        }catch(const std::exception& error){output.put("exception",sqlExceptionText(error));}
        output.put("before",integer(*config,"now",1700000000000LL));output.put("after",integer(*config,"now",1700000000000LL));output.put("clockCalls",clockCalls);
        output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()?JSONValue(db.getSQLFile()->string()):JSONValue());output.put("retention",TxPoWSqlDB::maxSQLMilli().load());outputs.add(output);
    }
    JSONObject result;result.put("ops",outputs);result.put("defaultRetention",defaultRetention);return result;
}
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try {std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}
}
