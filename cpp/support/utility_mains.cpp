#include "org/minima/utils/Maths.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/kissvm/functions/number/POW.hpp"
#include "org/minima/kissvm/functions/string/REPLACE.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
struct Raw:JSONObject {std::string text;mutable int renders=0;explicit Raw(std::string value):text(std::move(value)){}std::string toJSONString()const override{++renders;return text;}};
struct Receiver:MinimaLogger::MainReceiver {int notifications=0;void PostNotifyEvent(const std::string&,std::shared_ptr<JSONObject>)override{++notifications;}};
int main(int argc,char** argv)try{
    if(argc<2)return 2;std::string name=argv[1];MinimaLogger::Runtime logger;
    logger.println=[](const auto& line){auto text=unicode::encodeUTF8(line);std::cout<<text.substr(text.find(" : ")+3)<<std::endl;};MinimaLogger::setRuntime(logger);
    if(name=="debug"||name=="hang"){
        if(argc!=5)return 2;Raw raw(argv[2]);bool debug=std::string(argv[3])=="true";int fail=std::stoi(argv[4]),calls=0;JSONArray logs;auto receiver=std::make_shared<Receiver>();logger.getMain=[receiver]{return receiver;};
        logger.println=[&](const auto& line){auto text=unicode::encodeUTF8(line);logs.add(text.substr(text.find(" : ")+3));if(++calls==fail)throw JavaTypedException("java.lang.IllegalStateException","logger");};MinimaLogger::setRuntime(logger);
        JSONObject result;std::string error="none";JSONValue output;
        if(name=="hang")std::cout<<"ENTER"<<std::endl;
        try{output=MiniFormat::JSONPretty(raw,debug);}catch(const std::exception& e){error=javaExceptionText(e);}
        result.put("output",output);result.put("error",error);result.put("logs",logs);result.put("notifications",receiver->notifications);result.put("renders",raw.renders);MinimaLogger::setRuntime({});std::cout<<result.toJSONString()<<std::endl;return 0;
    }
    if(name=="Maths")Maths::main();else if(name=="BaseConverter")BaseConverter::main();else if(name=="MiniFormat")MiniFormat::main();else if(name=="POW")kissvm::POW::main();else if(name=="REPLACE")kissvm::REPLACE::main();else return 2;
}catch(const std::exception& e){std::cout<<"EXCEPTION:"<<javaExceptionText(e)<<std::endl;}
