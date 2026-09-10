#include "SqlRecordingFixture.hpp"
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <fstream>
#include <iostream>
using namespace minima;
using namespace minima::json;
namespace {
using namespace minima::sql_fixture;
class DB:public SqlDB {
    void createSQL() override {event("createSQL");}
public:
    DB():SqlDB({[](const std::string& url,const std::string& user,const std::string& password)->std::shared_ptr<SqlConnection>{
        event("connect",{url,user,password});if(flag("nullConnection"))throw SqlException("No suitable driver found for "+url);return std::make_shared<Connection>();
    },[]{return flag("debug");},[](const std::string& text,bool notify){
        if(text.starts_with("ExecuteSQL sql:")&&notify)throw std::logic_error("SQL errors must not notify");logs.add(text);
    }}){}
    void clear() {mSQLConnection.reset();}
};
SqlDB::File file(const JSONObject& op,const std::string& root) {
    if(!op.containsKey("file"))return std::filesystem::path(root+"/db/main");
    auto value=string(op.get("file"));return value?SqlDB::File(*value):std::nullopt;
}
JSONObject run(const JSONObject& input,const std::string& root) {
    connectionID=0;DB db;JSONArray outputs;
    for(const auto& entry:array(input.get("ops")).elements()) {
        config=&object(entry);events=JSONArray();logs=JSONArray();calls=0;JSONObject output;auto f=file(*config,root);
        if(flag("touch")){std::filesystem::create_directories(f->parent_path());std::ofstream out(*f);out.put('*');}
        try {
            JSONValue value;auto cmd=config->get("cmd").asString();
            if(cmd=="load")db.loadDB(f);
            else if(cmd=="encrypted")db.loadEncryptedSQLDB(f,string(config->get("password")));
            else if(cmd=="open")value=db.isOpen();
            else if(cmd=="check")value=db.checkOpen(config->get("logs").isNull()||config->get("logs").asBool());
            else if(cmd=="hard")db.hardCloseDB();
            else if(cmd=="save")db.saveDB(flag("compact"));
            else if(cmd=="backup")db.backupToFile(f,flag("gzip"));
            else if(cmd=="restore")db.restoreFromFile(f,flag("gzip"));
            else if(cmd=="sql")value=db.executeGenericSQL(string(config->get("sql")));
            else if(cmd=="clear")db.clear();
            else if(cmd=="decode"){auto result=SqlDB::convertDataToJSONObject(MiniData::fromHex(config->get("hex").asString()));if(result)value=*result;}
            else if(cmd=="encode")value=SqlDB::convertJSONObjectToData(config->get("json").isNull()?nullptr:&object(config->get("json"))).to0xString();
            else throw std::invalid_argument(cmd);
            output.put("value",value);
        } catch(const std::exception& error) {output.put("exception",sqlExceptionText(error));}
        output.put("logs",logs);output.put("events",events);output.put("file",db.getSQLFile()?JSONValue(db.getSQLFile()->string()):JSONValue());
        std::error_code error;output.put("exists",f&&f->string().find('\0')==std::string::npos&&std::filesystem::exists(f->empty()?".":*f,error));outputs.add(output);
    }
    JSONObject result;result.put("ops",outputs);return result;
}
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line))try {std::cout<<run(object(JSONValue::parseWithException(line)),argv[1]).toJSONString()<<'\n';}
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}
}
