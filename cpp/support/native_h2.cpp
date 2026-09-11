#include "support/NativeH2Connection.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/SqlDB.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <bit>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
using namespace minima;
using namespace minima::json;
namespace {
class DB final:public SqlDB {void createSQL() override{};public:explicit DB(SqlConnect connect):SqlDB({std::move(connect),[]{return false;},[](const auto&,bool){}}){}};
std::optional<std::string> text(const JSONObject& step,const char* key){auto value=step.get(key);return value.isNull()?std::nullopt:std::optional(value.asString());}
int32_t number(const JSONObject& step,const char* key){return int32_t(std::get<int64_t>(step.get(key).value));}
bool flag(const JSONObject& step,const char* key){auto value=step.get(key);return !value.isNull()&&value.asBool();}
std::string units(const std::u16string& value){std::ostringstream out;out<<std::hex<<std::uppercase<<std::setfill('0');for(char16_t c:value)out<<std::setw(4)<<int(c);return out.str();}
std::string bits(double value){std::ostringstream out;out<<std::hex<<std::uppercase<<std::setw(16)<<std::setfill('0')<<std::bit_cast<uint64_t>(value);return out.str();}
JSONArray run(const JSONObject& request,NativeH2Runtime& engine){
    auto connect=engine.connector();DB db(connect);
    std::map<std::string,std::shared_ptr<SqlConnection>> connections;
    std::map<std::string,std::shared_ptr<SqlStatement>> statements;
    std::map<std::string,std::shared_ptr<SqlResultSet>> rows;
    std::map<std::string,std::shared_ptr<SqlResultSetMetaData>> metadata;
    std::map<std::string,std::shared_ptr<SqlObject>> objects;JSONArray output;
    for(const auto& raw:std::get<std::shared_ptr<JSONArray>>(request.get("ops").value)->elements()){
        auto step=std::get<std::shared_ptr<JSONObject>>(raw.value);auto op=step->getString("op"),id=step->getString("id",""),save=step->getString("save","");JSONObject result;JSONValue value;
        try{
            auto prepared=[&]{auto p=std::dynamic_pointer_cast<SqlPreparedStatement>(statements.at(id));if(!p)throw JavaClassCastException("Statement is not prepared");return p;};
            if(op=="version")value=engine.version();
            else if(op=="open")connections[save]=connect(step->getString("url"),step->getString("user"),step->getString("password"));
            else if(op=="closed")value=connections.at(id)->isClosed();
            else if(op=="autocommit")connections.at(id)->setAutoCommit(flag(*step,"value"));
            else if(op=="statement")statements[save]=connections.at(id)->createStatement();
            else if(op=="prepare")statements[save]=connections.at(id)->prepareStatement(step->getString("sql"));
            else if(op=="preparedType")value=bool(std::dynamic_pointer_cast<SqlPreparedStatement>(statements.at(id)));
            else if(op=="execute")value=statements.at(id)->execute(text(*step,"sql"));
            else if(op=="query")rows[save]=statements.at(id)->executeQuery(text(*step,"sql"));
            else if(op=="result"){rows[save]=statements.at(id)->getResultSet();value=!rows.at(save);}
            else if(op=="closeStatement")statements.at(id)->close();
            else if(op=="clear")prepared()->clearParameters();
            else if(op=="bind"){
                auto statement=prepared();int32_t index=number(*step,"index");auto kind=step->getString("kind");
                if(kind=="string")statement->setString(index,text(*step,"value"));
                else if(kind=="int")statement->setInt(index,number(*step,"value"));
                else if(kind=="long")statement->setLong(index,std::stoll(step->getString("value")));
                else if(kind=="double")statement->setDouble(index,std::bit_cast<double>(std::stoull(step->getString("value"),nullptr,16)));
                else if(kind=="bytes"){auto v=text(*step,"value");statement->setBytes(index,v?std::optional(MiniData::fromHex(*v).bytes()):std::nullopt);}
            }
            else if(op=="executePrepared")value=prepared()->execute();
            else if(op=="queryPrepared")rows[save]=prepared()->executeQuery();
            else if(op=="update")value=prepared()->executeUpdate();
            else if(op=="next")value=rows.at(id)->next();
            else if(op=="string"){auto v=rows.at(id)->getString(step->getString("column"));if(v)value=units(unicode::decodeUTF8(*v));}
            else if(op=="int")value=rows.at(id)->getInt(step->getString("column"));
            else if(op=="long")value=std::to_string(rows.at(id)->getLong(step->getString("column")));
            else if(op=="bytes"){auto v=rows.at(id)->getBytes(step->getString("column"));if(v)value=MiniData(*v).toHexString(false);}
            else if(op=="meta")metadata[save]=rows.at(id)->getMetaData();
            else if(op=="columns")value=metadata.at(id)->getColumnCount();
            else if(op=="columnName")value=units(metadata.at(id)->getColumnName(number(*step,"index")));
            else if(op=="columnClass"){auto v=metadata.at(id)->getColumnClassName(number(*step,"index"));if(v)value=*v;}
            else if(op=="object"){
                auto v=rows.at(id)->getObject(number(*step,"index"));objects[save]=v;
                value=!v?"null":std::dynamic_pointer_cast<SqlDouble>(v)?"double":std::dynamic_pointer_cast<SqlClob>(v)?"clob":"object";
            }
            else if(op=="text")value=units(objects.at(id)->toString());
            else if(op=="double")value=bits(std::dynamic_pointer_cast<SqlDouble>(objects.at(id))->doubleValue());
            else if(op=="clobLength")value=std::to_string(std::dynamic_pointer_cast<SqlClob>(objects.at(id))->length());
            else if(op=="clobString")value=units(std::dynamic_pointer_cast<SqlClob>(objects.at(id))->getSubString(std::stoll(step->getString("start")),number(*step,"length")));
            else if(op=="closeConnection")connections.at(id)->close();
            else if(op=="releaseRow")rows.erase(id);
            else if(op=="dbLoad")db.loadDB(step->getString("file"));
            else if(op=="dbEncrypted")db.loadEncryptedSQLDB(step->getString("file"),text(*step,"password"));
            else if(op=="dbSQL")value=db.executeGenericSQL(text(*step,"sql"));
            else if(op=="dbBackup")db.backupToFile(step->getString("file"),flag(*step,"gzip"));
            else if(op=="dbRestore")db.restoreFromFile(step->getString("file"),flag(*step,"gzip"));
            else if(op=="dbSave")db.saveDB(flag(*step,"compact"));
            else if(op=="dbClose")db.hardCloseDB();
            else if(op=="dbOpen")value=db.isOpen();
            else if(op=="dbCheck")value=db.checkOpen();
            else throw std::invalid_argument(op);
            result.put("value",value);
        }catch(const std::exception& error){result.put("exception",nativeExceptionText(error));}
        output.add(result);
    }
    db.hardCloseDB();for(const auto& [id,connection]:connections)connection->close();return output;
}
}
int main(){try{
    NativeH2Runtime engine;std::string line;
    while(std::getline(std::cin,line)){
        auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);auto result=run(*request,engine);
        if(engine.liveHandles()!=0)throw std::runtime_error("Native H2 handle leak: "+std::to_string(engine.liveHandles()));
        std::cout<<result.toJSONString()<<std::endl;
    }
}catch(const std::exception& error){std::cerr<<nativeExceptionText(error)<<std::endl;return 1;}}
