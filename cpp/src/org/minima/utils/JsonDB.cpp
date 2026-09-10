#include "JsonDB.hpp"
#include "support/JavaExceptions.hpp"
#include "json/parser/JSONParser.hpp"
#include "json/parser/ParseException.hpp"
namespace minima {
bool JsonDB::exists(const std::string& name) const {return !params().get(name).isNull();}
bool JsonDB::getBoolean(const std::string& name,bool fallback) const {if(!exists(name))return fallback;auto value=std::get_if<bool>(&params().get(name).value);if(!value)throw JavaClassCastException("Cannot cast stored value to Boolean");return *value;}
void JsonDB::setBoolean(const std::string& name,bool value) {params().put(name,value);}
MiniNumber JsonDB::getNumber(const std::string& name,const MiniNumber& fallback) const {
    if(!exists(name))return fallback;
    const auto& value=params().get(name);if(!value.isString())throw JavaClassCastException("Cannot cast stored number to String");
    return MiniNumber(value.asString());
}
void JsonDB::setNumber(const std::string& name,const MiniNumber& value) {params().put(name,value.toString());}
MiniData JsonDB::getData(const std::string& name,const MiniData& fallback) const {return exists(name)?MiniData::fromHex(params().get(name).asString()):fallback;}
void JsonDB::setData(const std::string& name,const MiniData& value) {params().put(name,value.to0xString());}
std::optional<std::string> JsonDB::getString(const std::string& name) const {if(!exists(name))return std::nullopt;const auto& value=params().get(name);if(!value.isString())throw JavaClassCastException("Cannot cast stored value to String");return value.asString();}
std::string JsonDB::getString(const std::string& name,const std::string& fallback) const {return getString(name).value_or(fallback);}
void JsonDB::setString(const std::string& name,const std::optional<std::string>& value) {params().put(name,value?json::JSONValue(*value):json::JSONValue(nullptr));}
void JsonDB::setJSON(const std::string& name,std::shared_ptr<json::JSONObject> value) {json::JSONValue json;if(value)json.value=std::move(value);params().put(name,std::move(json));}
std::shared_ptr<json::JSONObject> JsonDB::getJSON(const std::string& name,std::shared_ptr<json::JSONObject> fallback) const {return exists(name)?std::get<std::shared_ptr<json::JSONObject>>(params().get(name).value):std::move(fallback);}
void JsonDB::setJSONArray(const std::string& name,std::shared_ptr<json::JSONArray> value) {json::JSONValue json;if(value)json.value=std::move(value);params().put(name,std::move(json));}
std::shared_ptr<json::JSONArray> JsonDB::getJSONArray(const std::string& name) {if(!exists(name))setJSONArray(name,std::make_shared<json::JSONArray>());return std::get<std::shared_ptr<json::JSONArray>>(params().get(name).value);}
void JsonDB::loadDB(const std::filesystem::path& file) {MiniFile::loadObjectSlow(file,*this);}
void JsonDB::saveDB(const std::filesystem::path& file) {MiniFile::saveObjectDirect(file,*this);}
std::vector<uint8_t> JsonDB::serialise() const {return MiniString(params().toString()).serialise();}
void JsonDB::readDataStream(std::span<const uint8_t> bytes,size_t& offset) {
    auto data=MiniString::deserialise(bytes,offset);
    try {auto value=json::parser::JSONParser().parse(data.toString());mParams=value.isNull()?nullptr:std::get<std::shared_ptr<json::JSONObject>>(value.value);}
    catch(const json::parser::ParseException& e){std::clog<<e.what()<<'\n';mParams=std::make_shared<json::JSONObject>();}
}
}
