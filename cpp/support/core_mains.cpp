#include "org/minima/system/commands/send/send.hpp"
#include "org/minima/database/txpowtree/TxPoWTreeNode.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
struct Receiver:MinimaLogger::MainReceiver {int calls=0;void PostNotifyEvent(const std::string&,std::shared_ptr<json::JSONObject>)override{++calls;}};
int main(int argc,char** argv) {
    if(argc<3)return 2;std::string name=argv[1],mode=argv[2];std::ostringstream output;auto previous=std::cout.rdbuf(output.rdbuf());
    auto receiver=std::make_shared<Receiver>();MinimaLogger::Runtime logger;logger.getMain=[receiver]{return receiver;};logger.println=[](const auto& line){auto text=unicode::encodeUTF8(line);std::cout<<text.substr(text.find(" : ")+3)<<std::endl;};MinimaLogger::setRuntime(logger);
    json::JSONArray events;std::string error="none";
    p2p::P2PFunctions::openURL=[&](const auto& url){for(const auto& stage:std::vector<std::string>{"open:"+url,"connect","input","close"}){events.add(stage);if(argc>4&&stage.starts_with(argv[3])){std::string kind=argv[4];if(kind=="error")throw JavaError("java.lang.AssertionError","fixture");if(kind=="oom")throw JavaOutOfMemoryError("fixture");throw wire::IOException("fixture");}}};
    try {
        std::vector<std::string> args;if(mode=="args")args={"unused","value"};auto ptr=mode=="null"?nullptr:&args;
        if(name=="send")commands::send::main(ptr);else if(name=="TxPoWTreeNode")TxPoWTreeNode::main(ptr);else if(name=="Contract")kissvm::Contract::main(ptr);else if(name=="P2PFunctions")p2p::P2PFunctions::main(ptr);else throw std::invalid_argument("unknown main");
    }catch(const std::exception& e){error=javaExceptionText(e);}
    p2p::P2PFunctions::openURL={};MinimaLogger::setRuntime({});std::cout.rdbuf(previous);
    json::JSONObject result;result.put("stdout",output.str());result.put("error",error);result.put("events",events);result.put("notifications",receiver->calls);std::cout<<result.toString()<<std::endl;
}
