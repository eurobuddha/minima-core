#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaExceptions.hpp"
#include <bit>
#include <iostream>
using namespace minima;
using namespace minima::json;
namespace {
std::shared_ptr<JSONObject> input;JSONArray trace;bool mainPresent,nested;int dates,prints,posts;
bool flag(const char* key){return input->containsKey(key)&&input->get(key).asBool();}
MinimaLogger::String string(const char* key){const auto& v=input->get(key);return v.isNull()?std::nullopt:std::optional(v.asUTF16());}
void record(const char* name){trace.add(name);}
struct Error:MinimaLogger::Throwable {
    MinimaLogger::String toString() const override{record("toString");if(flag("failString"))throw std::runtime_error("fixture toString");return string("text");}
    std::optional<std::vector<MinimaLogger::String>> getStackTrace() const override{
        record("stack");if(flag("failStack"))throw std::runtime_error("fixture stack");if(flag("nullFrames"))return std::nullopt;
        std::vector<MinimaLogger::String> frames;for(const auto& v:std::get<std::shared_ptr<JSONArray>>(input->get("frames").value)->elements())frames.push_back(v.isNull()?std::nullopt:std::optional(u"Fixture."+v.asUTF16()+u"(Fixture.java:12)"));return frames;
    }
};
struct Main:MinimaLogger::MainReceiver {
    void PostNotifyEvent(const std::string& event,std::shared_ptr<JSONObject> data) override {
        JSONObject row;row.put("event",event);JSONValue value;value.value=data;row.put("data",value);trace.add(row);++posts;
        if(flag("mutatePost"))data->put("after",true);
        if(flag("clearPost"))mainPresent=false;
        if(flag("nested")&&!nested){nested=true;MinimaLogger::log("nested",false);}
        if(flag("failPost"))throw std::runtime_error("fixture post");
    }
};
JSONObject run(std::shared_ptr<JSONObject> request){
    input=std::move(request);trace=JSONArray();mainPresent=flag("main");nested=false;dates=prints=posts=0;
    JSONObject result;
    if(input->containsKey("date")){result.put("date",MinimaLogger::DATEFORMAT.format(std::stoll(input->getString("date"))));return result;}
    auto receiver=std::make_shared<Main>();MinimaLogger::Runtime runtime;runtime.getMain=[receiver]{return mainPresent?receiver:nullptr;};
    runtime.totalMemory=[]{return int64_t(1048576);};runtime.freeMemory=[]{return int64_t(2048);};
    runtime.currentTimeMillis=[]{record("date");if(flag("failDate"))throw std::runtime_error("fixture date");return int64_t(1700000000000)+1000*dates++;};
    runtime.println=[](const auto& value){JSONObject row;row.put("print",value);trace.add(row);++prints;if(flag("clearPrint"))mainPresent=false;if(flag("setPrint"))mainPresent=true;if(flag("failPrint"))throw std::runtime_error("fixture print");};
    MinimaLogger::setRuntime(std::move(runtime));
    try{
        auto mode=input->getString("mode");bool notify=flag("notify");std::shared_ptr<MinimaLogger::Throwable> error=flag("nullError")?nullptr:std::make_shared<Error>();
        if(mode=="string")MinimaLogger::log(string("text"),notify);
        else if(mode=="defaultString")MinimaLogger::log(string("text"));
        else if(mode=="exception")MinimaLogger::log(error,notify);
        else if(mode=="defaultException")MinimaLogger::log(error);
        else if(mode=="uncaught")MinimaLogger::logUncaught(error,notify);
        else throw std::invalid_argument(mode);
    }catch(const std::exception& e){result.put("exception",javaExceptionText(e));}
    result.put("trace",trace);result.put("prints",prints);result.put("posts",posts);result.put("main",mainPresent);return result;
}
}
int main(){std::string line;while(std::getline(std::cin,line))try{auto value=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<run(value).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
