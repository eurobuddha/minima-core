#include "org/minima/Minima.hpp"
#include "MainFixture.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
std::mutex starterOutputLock;
class ManualMain final:public Main {public:ManualMain():Main(MainServices{}){}};
class StarterHandler final:public NativeUncaughtExceptionHandler {
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& error) override{
        auto name=unicode::encodeUTF8(error->toString().value());
        std::lock_guard guard(starterOutputLock);
        std::cout<<"uncaught:"<<unicode::encodeUTF8(thread->name)<<":"<<name.substr(0,name.find(':'))<<std::endl;
    }
};
int main(int argc,char** argv){
    if(argc>1){
        setDefaultUncaughtExceptionHandler(std::make_shared<StarterHandler>());
        std::shared_ptr<Minima::Args> args;
        if(std::string(argv[1])!="starter-null")args=std::make_shared<Minima::Args>(Minima::Args{std::string(argv[1])=="starter-help"?u"-help":u"-entry-unknown"});
        Minima().mainStarter(args);
        if(!args){std::lock_guard guard(starterOutputLock);std::cout<<"RETURNED"<<std::endl;}
        Minima::joinStarterThreads();return 0;
    }
    Minima instance;std::string line;
    while(std::getline(std::cin,line)){
        auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);JSONObject output;
        try{
            if(request->getString("op","")=="singleton"){
                auto previous=Main::getInstance();auto owner=std::make_shared<ManualMain>();MainFixture::install(owner);
                output.put("same",Minima::getMain()==owner);MainFixture::install(nullptr);output.put("cleared",!Minima::getMain());MainFixture::install(previous);
            }else{
                auto string=[&](const char* key)->Minima::String{auto value=request->get(key);return value.isNull()?std::nullopt:std::optional(value.asUTF16());};
                auto text=string("text");int arity=int(std::get<int64_t>(request->get("arity").value));bool pretty=request->get("pretty").isNull()?false:request->get("pretty").asBool();
                output.put("result",arity==1?instance.runMinimaCMD(text):arity==2?instance.runMinimaCMD(text,pretty):instance.runMinimaCMD(text,pretty,string("caller")));
            }
        }catch(const std::exception& error){output.put("exception",nativeExceptionText(error));}
        std::cout<<output.toJSONString()<<std::endl;
    }
}
