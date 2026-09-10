#include "org/minima/database/userprefs/UserDB.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <iostream>
using namespace minima;
static std::optional<std::string> nullable(const json::JSONValue& v){return v.isNull()?std::nullopt:std::optional(v.asString());}
static auto array(const json::JSONValue& v){return v.isNull()?nullptr:std::get<std::shared_ptr<json::JSONArray>>(v.value);}
static auto object(const json::JSONValue& v){return v.isNull()?nullptr:std::get<std::shared_ptr<json::JSONObject>>(v.value);}
static std::string render(const std::shared_ptr<json::JSONObject>& v){return v?v->toJSONString():"null";}
static std::string render(const std::shared_ptr<json::JSONArray>& v){return v?v->toJSONString():"null";}
static std::string render(const std::optional<std::string>& v){return v?json::JSONValue(*v).toJSONString():"null";}
static std::string render(const std::string& v){return json::JSONValue(v).toJSONString();}
static std::string render(bool v){return v?"true":"false";}
static std::string render(const MiniNumber& v){return render(v.toString());}
static std::string render(const MiniData& v){return render(v.to0xString());}
static std::string render(const MiniUtil::StringList& v){return render(MiniUtil::convertArrayList(v));}
static std::string call(UserDB& db,const json::JSONArray& args){auto name=args.get(0).asString();
    if(name=="wire")return render(BaseConverter::encode16(db.serialise()));
    if(name=="alias"){auto a=std::make_shared<json::JSONArray>();db.setJSONArray("a",a);a->add("first");auto original=db.getJSONArray("a");db.setJSONArray("b",original);original->add(nullptr);db.getJSONArray("b")->add("last");return render(db.getAllData());}
#include "prefs_dispatch.hpp"
    throw std::invalid_argument(name);
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{
    auto request=object(json::parser::JSONParser().parse(line));UserDB::language=request->getString("locale");UserDB db;db.setString("keep","original");std::string out;
    if(request->containsKey("date")){std::cout<<MiniUtil::DATEFORMAT.format(std::stoll(request->getString("date")))<<'\n';continue;}
    try{auto bytes=MiniString(request->getString("initial")).serialise();size_t offset=0;db.readDataStream(bytes,offset);out="null";}catch(const std::exception&){out="ERROR";}
    for(const auto& command:array(request->get("commands"))->elements())try{out+="|"+call(db,*array(command));}catch(const std::exception&){out+="|ERROR";}
    out+="|"+render(db.getAllData());std::cout<<out<<'\n';
}catch(const std::exception&){std::cout<<"ERROR\n";}}
