#include "org/minima/utils/MinimaUncaughtException.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace {
std::shared_ptr<JSONObject> input;JSONArray trace;int strings=0,prints=0,dates=0;
bool flag(const char* key){return input->containsKey(key)&&input->get(key).asBool();}
int number(const char* key,int fallback=0){return input->containsKey(key)?int(std::get<int64_t>(input->get(key).value)):fallback;}
struct Error:MinimaLogger::Throwable{
    using String=MinimaLogger::String;
    String toString()const override{
        trace.add("toString");++strings;if(strings==number("failStringAt",-1))throw std::runtime_error("fixture toString");
        auto value=input->get("text");if(input->containsKey("texts")){auto array=std::get<std::shared_ptr<JSONArray>>(input->get("texts").value);value=array->get(std::min(size_t(strings-1),array->size()-1));}
        return value.isNull()?std::nullopt:String(value.asUTF16());
    }
    std::optional<std::vector<String>> getStackTrace()const override{
        trace.add("stack");if(flag("failStack"))throw std::runtime_error("fixture stack");if(flag("nullFrames"))return std::nullopt;
        std::vector<String> result;if(input->containsKey("frames"))for(const auto& value:std::get<std::shared_ptr<JSONArray>>(input->get("frames").value)->elements())result.push_back(value.isNull()?std::nullopt:String(u"Fixture."+value.asUTF16()+u"(Fixture.java:12)"));return result;
    }
    Kind kind()const noexcept override{
        auto k=input->getString("kind","exception");if(k=="oom")return Kind::OutOfMemory;if(k=="store")return Kind::MVStore;if(k=="timeout")return Kind::Timeout;if(k=="error")return Kind::Error;return Kind::Exception;
    }
};
struct Handler:NativeUncaughtExceptionHandler{
    std::u16string name;explicit Handler(std::u16string n):name(std::move(n)){}
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>&)override{trace.add(u"delegate:"+name+u":"+thread->name);if(flag("failDelegate"))throw std::runtime_error("fixture delegate");}
};
struct Main:MinimaLogger::MainReceiver{
    void PostNotifyEvent(const std::string& event,std::shared_ptr<JSONObject> data)override{JSONObject row;row.put("event",event);JSONValue value;value.value=data;row.put("data",value);trace.add(row);if(flag("failPost"))throw std::runtime_error("fixture post");}
};
void setup(bool capture){
    MinimaLogger::Runtime r;r.totalMemory=[]{return int64_t(2048);};r.freeMemory=[]{return int64_t(1024);};r.currentTimeMillis=[]{trace.add("date");if(flag("failDate"))throw std::runtime_error("fixture date");return int64_t(1700000000000)+1000*dates++;};auto main=std::make_shared<Main>();r.getMain=[main]{return flag("main")?main:nullptr;};
    if(capture)r.println=[](const auto& text){JSONObject row;row.put("print",text);trace.add(row);if(++prints==number("failPrintAt",-1))throw std::runtime_error("fixture print");};MinimaLogger::setRuntime(r);
}
JSONObject program(std::shared_ptr<JSONObject> request){
    setup(true);setDefaultUncaughtExceptionHandler(nullptr);std::vector<std::shared_ptr<MinimaUncaughtException>> handlers;JSONArray rows;
    for(const auto& op:std::get<std::shared_ptr<JSONArray>>(request->get("ops").value)->elements()){
        input=std::get<std::shared_ptr<JSONObject>>(op.value);trace=JSONArray();strings=prints=dates=0;JSONObject row;
        try{
            auto mode=input->getString("op");if(mode=="default")setDefaultUncaughtExceptionHandler(input->get("name").isNull()?nullptr:std::make_shared<Handler>(input->get("name").asUTF16()));
            else if(mode=="bind")setDefaultUncaughtExceptionHandler(handlers.at(number("id")));
            else if(mode=="new")handlers.push_back(std::make_shared<MinimaUncaughtException>());
            else if(mode=="call"){NativeThread thread{input->containsKey("thread")?input->get("thread").asUTF16():u"worker"};handlers.at(number("id"))->uncaughtException(flag("nullThread")?nullptr:&thread,flag("nullError")?nullptr:std::make_shared<Error>());}
            else throw std::invalid_argument("op");
        }catch(const std::exception& e){row.put("exception",nativeExceptionText(e));}
        row.put("trace",trace);rows.add(row);
    }
    setDefaultUncaughtExceptionHandler(nullptr);MinimaLogger::setRuntime({});JSONObject out;out.put("rows",rows);return out;
}
}
int main(int argc,char** argv){
    if(argc>1){input=std::make_shared<JSONObject>();input->put("kind",argv[1]);input->put("text","fatal-fixture");setup(false);std::atexit([]{std::cout<<"SHUTDOWN-HOOK\n";});auto handler=std::make_shared<MinimaUncaughtException>();
        if(argc>2){setDefaultUncaughtExceptionHandler(handler);std::thread worker([&]{NativeThread thread{u"fatal-worker"};runNativeThread(thread,[&]{std::string kind=argv[1];if(kind=="oom")throw JavaOutOfMemoryError("native-carrier");if(kind=="store")throw JavaMVStoreException("native-carrier");if(kind=="timeout")throw JavaTimeoutException("native-carrier");});});worker.join();}
        else{NativeThread thread{u"fatal-worker"};handler->uncaughtException(&thread,std::make_shared<Error>());}std::cout<<"RETURNED\n";return 7;}
    std::string line;while(std::getline(std::cin,line))try{auto value=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);std::cout<<program(value).toJSONString()<<'\n';}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
