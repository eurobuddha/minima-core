#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
static JSONObject& object(const JSONValue& value){return *std::get<std::shared_ptr<JSONObject>>(value.value);}
static JSONArray& array(const JSONValue& value){return *std::get<std::shared_ptr<JSONArray>>(value.value);}
static JSONValue shared(const std::shared_ptr<JSONObject>& value){JSONValue result;result.value=value;return result;}
static JSONArray run(const JSONObject& input){
    bool list=input.getString("kind")=="array";
    auto a=std::make_shared<JSONArray>();auto o=std::make_shared<JSONObject>();
    std::shared_ptr<JSONArray> acopy;std::shared_ptr<JSONObject> ocopy;
    auto child=std::make_shared<JSONObject>();
    std::optional<decltype(a->iterator())> ai;std::optional<decltype(o->iterator())> oi;
    JSONArray output;
    for(const auto& raw:array(input.get("ops")).elements()){
        const auto& op=object(raw);auto action=op.getString("op");std::string result="ok";
        try{
            if(action=="add")a->add(op.get("value"));
            else if(action=="remove")result=JSONValue(a->removeString(op.getString("value"))).toJSONString();
            else if(action=="contains")result=JSONValue(a->containsString(op.getString("value"))).toJSONString();
            else if(action=="put")o->put(op.getString("key"),op.get("value"));
            else if(action=="get")result=(list?a->get(size_t(std::get<int64_t>(op.get("index").value))):o->get(op.getString("key"))).toJSONString();
            else if(action=="iter"){if(list)ai.emplace(a->iterator());else oi.emplace(o->iterator());}
            else if(action=="has")result=JSONValue(list?ai->hasNext():oi->hasNext()).toJSONString();
            else if(action=="next"){
                if(list)result=ai->next().toJSONString();
                else{auto [key,value]=oi->next();JSONArray pair;pair.add(key);pair.add(value);result=pair.toJSONString();}
            }else if(action=="serial")result=list?a->toJSONString():o->toJSONString();
            else if(action=="copy"){if(list)acopy=std::make_shared<JSONArray>(*a);else ocopy=std::make_shared<JSONObject>(*o);}
            else if(action=="copySerial")result=list?acopy->toJSONString():ocopy->toJSONString();
            else if(action=="replace"){if(list)a=std::make_shared<JSONArray>();else o=std::make_shared<JSONObject>();}
            else if(action=="childPut")child->put(op.getString("key"),op.get("value"));
            else if(action=="childStore"){if(list)a->add(shared(child));else o->put(op.getString("key"),shared(child));}
            else throw std::invalid_argument("Unknown operation");
        }catch(const std::exception& e){result=javaExceptionText(e);result=result.substr(0,result.find(':'));}
        output.add(result);
    }
    return output;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(object(JSONValue::parseWithException(line))).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
