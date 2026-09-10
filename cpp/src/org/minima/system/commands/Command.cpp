#include "Command.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
json::JSONObject Command::getJSONReply() const {
    json::JSONObject result;result.put("command",mName?json::JSONValue(*mName):json::JSONValue());
    if(mParams->size()){json::JSONValue params;params.value=mParams;result.put("params",std::move(params));}
    result.put("status",true);result.put("pending",false);return result;
}
bool Command::existsParam(const std::string& name) const {return mParams->containsKey(name);}
std::string Command::getParam(const std::string& name) const {
    return unicode::encodeUTF8(getParamUTF16(name));
}
std::string Command::getParam(const std::string& name,const std::string& fallback) const {return existsParam(name)?getParam(name):fallback;}
std::u16string Command::getParamUTF16(const std::string& name) const {
    if(!existsParam(name))throw CommandException("param not specified : "+name);
    const auto& input=mParams->get(name);if(input.isNull())throw JavaNullPointerException("null command parameter");if(!input.isString())throw JavaClassCastException("Cannot cast command parameter to String");
    auto value=input.asUTF16();size_t start=0,end=value.size();while(start<end&&value[start]<=32)++start;while(end>start&&value[end-1]<=32)--end;
    if(start==end)throw CommandException("BLANK param not allowed : "+name);return value.substr(start,end-start);
}
std::u16string Command::getParamUTF16(const std::string& name,const std::u16string& fallback) const {return existsParam(name)?getParamUTF16(name):fallback;}
std::optional<std::string> Command::getParam(const std::string& name,std::nullptr_t) const {return existsParam(name)?std::optional(getParam(name)):std::nullopt;}
bool Command::getBooleanParam(const std::string& name) const {return getParam(name)=="true";}
bool Command::getBooleanParam(const std::string& name,bool fallback) const {return existsParam(name)?getBooleanParam(name):fallback;}
MiniNumber Command::getNumberParam(const std::string& name) const {return MiniNumber(getParam(name));}
MiniNumber Command::getNumberParam(const std::string& name,const MiniNumber& fallback) const {return existsParam(name)?getNumberParam(name):fallback;}
std::optional<MiniNumber> Command::getNumberParam(const std::string& name,std::nullptr_t) const {return existsParam(name)?std::optional(getNumberParam(name)):std::nullopt;}
MiniData Command::getDataParam(const std::string& name) const {
    auto value=getParam(name);try{if(javaLowerCase(value).starts_with("mx"))return Address::convertMinimaAddress(value);return MiniData::fromHex(value);}
    catch(const std::exception&){throw CommandException("Invalid Data param specified : "+name+" "+value);}
}
MiniData Command::getDataParam(const std::string& name,const MiniData& fallback) const {return existsParam(name)?getDataParam(name):fallback;}
std::optional<MiniData> Command::getDataParam(const std::string& name,std::nullptr_t) const {return existsParam(name)?std::optional(getDataParam(name)):std::nullopt;}
std::shared_ptr<json::JSONObject> Command::getJSONObjectParam(const std::string& name) const {
    if(!existsParam(name))throw CommandException("param not specified : "+name);
    const auto& value=mParams->get(name);if(value.isNull())return nullptr;
    if(auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value))return *object;
    throw JavaClassCastException("Cannot cast command parameter to JSONObject");
}
std::shared_ptr<json::JSONObject> Command::getJSONObjectParam(const std::string& name,std::shared_ptr<json::JSONObject> fallback) const {return existsParam(name)?getJSONObjectParam(name):std::move(fallback);}
std::shared_ptr<json::JSONArray> Command::getJSONArrayParam(const std::string& name) const {
    if(!existsParam(name))throw CommandException("param not specified : "+name);
    const auto& value=mParams->get(name);if(value.isNull())return nullptr;
    if(auto array=std::get_if<std::shared_ptr<json::JSONArray>>(&value.value))return *array;
    throw JavaClassCastException("Cannot cast command parameter to JSONArray");
}
std::string Command::getAddressParam(const std::string& name) const {
    auto address=getParam(name);
    if(javaLowerCase(address).starts_with("mx"))try{address=Address::convertMinimaAddress(address).to0xString();}
    catch(const JavaNumberFormatException& e){throw CommandException(std::string("java.lang.NumberFormatException: ")+e.what());}
    catch(const std::invalid_argument& e){throw CommandException(std::string("java.lang.IllegalArgumentException: ")+e.what());}
    if(address.starts_with("0x"))try{address=MiniData::fromHex(address).to0xString();}catch(const std::exception&){throw CommandException("java.lang.NumberFormatException: Invalid HEX string in decode16 : "+address);}
    return address;
}
std::string Command::getAddressParam(const std::string& name,const std::string& fallback) const {return existsParam(name)?getAddressParam(name):fallback;}
std::optional<std::string> Command::getAddressParam(const std::string& name,std::nullptr_t) const {return existsParam(name)?std::optional(getAddressParam(name)):std::nullopt;}
bool Command::isParamJSONObject(const std::string& name) const {return std::holds_alternative<std::shared_ptr<json::JSONObject>>(mParams->get(name).value);}
bool Command::isParamJSONArray(const std::string& name) const {return std::holds_alternative<std::shared_ptr<json::JSONArray>>(mParams->get(name).value);}
}
