#include "SqlRecordingFixture.hpp"
#include "ArchiveFixture.hpp"
#include "org/minima/database/archive/ArchiveManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
using namespace minima::sql_fixture;
namespace {
int clockCalls,shutdownCalls;
class DB:public ArchiveManager {
public:
    DB():ArchiveManager({[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const auto& text,bool){logs.add(text);}},[]{
        if(flag("nullMain"))throw SqlRuntimeException("java.lang.NullPointerException","null Main");
        ++shutdownCalls;return flag("shutdown")||shutdownCalls>=integer(*config,"shutdownAfter",INT32_MAX);
    },[]{++clockCalls;return integer(*config,"now",1700000000000LL);}){}
    void create(){createSQL();}
    void clear(){mSQLConnection.reset();}
};
JSONValue wireBlock(const std::shared_ptr<TxBlock>& block){return block?JSONValue(MiniData(block->serialise()).to0xString()):JSONValue();}
JSONValue wireCascade(const std::shared_ptr<Cascade>& cascade){return cascade?JSONValue(MiniData(cascade->serialise()).to0xString()):JSONValue();}
JSONArray wireBlocks(const ArchiveManager::Blocks& blocks){JSONArray out;for(const auto& block:blocks)out.add(wireBlock(block));return out;}
std::shared_ptr<Cascade> fixtureCascade(const JSONObject& spec){
    auto cascade=std::make_shared<Cascade>();for(int i=0;i<integer(spec,"count",3);++i)cascade->addToTip(*support::archiveFixtureTxPoW(i,0));
    auto tip=cascade->getTip();int mode=int(integer(spec,"mode"));
    if(tip){
        if(mode&1)tip->setLevel(3);
        if(mode&2)tip->getTxPoW().setSuperParent(1,MiniData::fromHex("0x1234"));
        if(mode&4)tip->getTxPoW().setSuperParent(0,MiniData::fromHex("0x1234"));
        if((mode&8)&&tip->getParent())tip->getParent()->setLevel(2);
    }return cascade;
}
JSONObject run(const JSONObject& input,const std::string& root){
    connectionID=preparedID=0;GeneralParams::NUMBER_DAYS_ARCHIVE=integer(input,"days",50);GeneralParams::ARCHIVE=false;DB db;JSONArray outputs,encoded,cascEncoded;
    std::vector<std::shared_ptr<TxBlock>> blocks;std::vector<std::shared_ptr<Cascade>> cascades;
    for(const auto& item:array(input.get("fixtures")).elements()){
        const auto& spec=object(item);auto block=std::make_shared<TxBlock>(*support::archiveFixtureTxPoW(int(integer(spec,"index")),int(integer(spec,"mode"))));
        if(spec.containsKey("nonce")){block->getTxPoW().setNonce(MiniNumber(integer(spec,"nonce")));block->getTxPoW().calculateTXPOWID();}
        blocks.push_back(block);encoded.add(wireBlock(block));
    }
    for(const auto& item:array(input.get("cascades")).elements()){auto cascade=fixtureCascade(object(item));cascades.push_back(cascade);cascEncoded.add(wireCascade(cascade));}
    int64_t defaultRetention=db.MAX_KEEP_BLOCKS.load();
    for(const auto& entry:array(input.get("ops")).elements()){
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=clockCalls=shutdownCalls=0;JSONObject output;
        if(config->containsKey("retention"))db.MAX_KEEP_BLOCKS=integer(*config,"retention");
        if(config->containsKey("archive"))GeneralParams::ARCHIVE=flag("archive");
        try{
            JSONValue result;auto cmd=config->get("cmd").asString();
            std::optional<MiniNumber> start,end;auto s=string(config->get("start")),e=string(config->get("end"));if(s)start.emplace(*s);if(e)end.emplace(*e);
            if(cmd=="load")db.loadDB(std::filesystem::path(root+"/db/archive"));
            else if(cmd=="create")db.create();
            else if(cmd=="clear")db.clear();
            else if(cmd=="hard")db.hardCloseDB();
            else if(cmd=="hack")db.hackShut();
            else if(cmd=="save")db.saveDB(flag("compact"));
            else if(cmd=="open")result=db.isOpen();
            else if(cmd=="size")result=db.getSize();
            else if(cmd=="add"){
                auto block=config->get("index").isNull()?nullptr:blocks.at(integer(*config,"index"));
                if(block&&flag("badHash"))block->getTxPoW().setSuperParent(0,MiniData(std::vector<uint8_t>(33,0)));
                result=db.saveBlock(block.get());
            }
            else if(cmd=="get")result=wireBlock(db.loadBlock(string(config->get("id"))));
            else if(cmd=="exists")result=db.exists(string(config->get("id"))).toString();
            else if(cmd=="number")result=wireBlock(db.loadBlockFromNumber(start?&*start:nullptr));
            else if(cmd=="first")result=wireBlock(db.loadFirstBlock());
            else if(cmd=="last")result=wireBlock(db.loadLastBlock());
            else if(cmd=="sync")result=wireBlocks(db.loadSyncBlockRange(start?&*start:nullptr));
            else if(cmd=="range")result=wireBlocks(config->containsKey("descending")?db.loadBlockRange(start?&*start:nullptr,end?&*end:nullptr,flag("descending")):db.loadBlockRange(start?&*start:nullptr,end?&*end:nullptr));
            else if(cmd=="cascade")result=wireCascade(db.loadCascade());
            else if(cmd=="checkCascade"){
                auto cascade=config->get("index").isNull()?nullptr:cascades.at(integer(*config,"index"));
                if(cascade&&cascade->getTip()&&flag("badHash"))cascade->getTip()->getTxPoW().setSuperParent(0,MiniData(std::vector<uint8_t>(33,0)));
                db.checkCascadeRequired(cascade.get());
            }
            else if(cmd=="clean")result=db.checkForCleanDB();
            else if(cmd=="reopen")db.closeAndReopen();
            else throw std::invalid_argument(cmd);
            output.put("value",result);
        }catch(const std::exception& error){output.put("exception",sqlExceptionText(error));}
        output.put("before",integer(*config,"now",1700000000000LL));output.put("after",integer(*config,"now",1700000000000LL));output.put("clockCalls",clockCalls);output.put("shutdownCalls",shutdownCalls);
        output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()?JSONValue(db.getSQLFile()->string()):JSONValue());output.put("retention",db.MAX_KEEP_BLOCKS.load());outputs.add(output);
    }
    JSONObject result;result.put("ops",outputs);result.put("fixtures",encoded);result.put("cascades",cascEncoded);result.put("defaultRetention",defaultRetention);return result;
}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}
}
