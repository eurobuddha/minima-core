#include "org/minima/database/wallet/SeedRow.hpp"
#include "org/minima/database/wallet/KeyRow.hpp"
#include "org/minima/database/wallet/ScriptRow.hpp"
#include <iostream>
using namespace minima;
static int32_t integer(const json::JSONObject& o,const std::string& key){const auto& value=o.get(key);return value.isNull()?0:int32_t(std::get<int64_t>(value.value));}
static auto string(const json::JSONObject& o,const std::string& key)->std::optional<std::string>{const auto& value=o.get(key);return value.isNull()?std::nullopt:std::optional(value.asString());}
static json::JSONValue value(const std::optional<std::string>& s){return s?json::JSONValue(*s):json::JSONValue();}
struct SqlError:std::runtime_error{using std::runtime_error::runtime_error;};
class Row:public SqlRow {
    const json::JSONObject& mFields;int mFail,mCalls=0;
    void called(const std::string& method,const std::string& key){log+=method+":"+key+",";if(++mCalls==mFail)throw SqlError("fixture failure");}
public:
    std::string log;Row(const json::JSONObject& fields,int fail):mFields(fields),mFail(fail){}
    int32_t getInt(const std::string& key) override{called("getInt",key);return integer(mFields,key);}
    std::optional<std::string> getString(const std::string& key) override{called("getString",key);return string(mFields,key);}
};
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto data=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);auto type=data->get("type").asString();bool sql=data->get("sql").asBool();auto fields=std::get<std::shared_ptr<json::JSONObject>>(data->get("fields").value);Row row(*fields,integer(*data,"fail"));std::string out;try{json::JSONObject getters,object;
    if(type=="seed"){auto r=sql?SeedRow(row):SeedRow(string(*fields,"phrase"),string(*fields,"seed"));getters.put("phrase",value(r.getPhrase()));getters.put("seed",value(r.getSeed()));object=r.toJSON();}
    else if(type=="key"){auto r=sql?KeyRow(row):KeyRow(integer(*fields,"size"),integer(*fields,"depth"),integer(*fields,"uses"),integer(*fields,"maxuses"),string(*fields,"modifier"),string(*fields,"privatekey"),string(*fields,"publickey"));getters.put("size",r.getSize());getters.put("depth",r.getDepth());getters.put("uses",r.getUses());getters.put("maxuses",r.getMaxUses());getters.put("modifier",value(r.getModifier()));getters.put("privatekey",value(r.getPrivateKey()));getters.put("publickey",value(r.getPublicKey()));object=r.toJSON();}
    else{auto r=sql?ScriptRow(row):ScriptRow(string(*fields,"script"),string(*fields,"address"),integer(*fields,"simple")!=0,integer(*fields,"defaultaddress")!=0,string(*fields,"publickey"),integer(*fields,"track")!=0);getters.put("script",value(r.getScript()));getters.put("address",value(r.getAddress()));getters.put("simple",r.isSimple());getters.put("default",r.isDefault());getters.put("publickey",value(r.getPublicKey()));getters.put("track",r.isTrack());object=r.toJSON();}
    out=getters.toJSONString()+"|"+object.toJSONString();
}catch(const SqlError&){out="SQL_ERROR";}catch(const std::exception&){out="ERROR";}std::cout<<out+"|"+row.log<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
