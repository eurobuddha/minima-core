#include "org/minima/system/brains/TxPoWMiner.hpp"
#include "org/minima/objects/keys/TreeKey.hpp"
#include "org/minima/database/cascade/Cascade.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
struct Receiver:MinimaLogger::MainReceiver {int calls=0;void PostNotifyEvent(const std::string&,std::shared_ptr<json::JSONObject>)override{++calls;}};
int main(int argc,char** argv) {
    if(argc<3)return 2;std::string name=argv[1],mode=argv[2];std::vector<std::string> args;if(mode=="args")args={"unused","value"};
    std::ostringstream output;auto previous=std::cout.rdbuf(output.rdbuf());auto receiver=std::make_shared<Receiver>();MinimaLogger::Runtime logger;
    logger.getMain=[receiver]{return receiver;};logger.println=[](const auto& line){auto text=unicode::encodeUTF8(line);std::cout<<text.substr(text.find(" : ")+3)<<std::endl;};MinimaLogger::setRuntime(logger);
    std::string error="none";
    try {auto input=mode=="null"?nullptr:&args;
        if(name=="TxPoWMiner")TxPoWMiner::main(input);
        else if(name=="TreeKey")TreeKey::main(input);
        else if(name=="Cascade")Cascade::main(input);
        else return 2;
    }catch(const std::exception& e){error=javaExceptionText(e);}
    MinimaLogger::setRuntime({});std::cout.rdbuf(previous);json::JSONObject result;result.put("stdout",output.str());result.put("error",error);result.put("notifications",receiver->calls);std::cout<<result.toString()<<'\n';
}
