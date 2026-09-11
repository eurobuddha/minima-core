#include "systemcheck.hpp"
#include "org/minima/system/Main.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/JavaStringIgnoreCase.hpp"
namespace minima::commands {
namespace {
std::shared_ptr<Main> main(){auto owner=Main::getInstance();sqlRequired(owner);return owner;}
}
systemcheck::systemcheck():Command("systemcheck","Check system processors.."){}
json::JSONValue systemcheck::runCommand(){
    auto result=getJSONReply();json::JSONObject response;
    auto action=getParam("action","list");
    if(action=="list"){
        response.put("Main",getInfo(Main::getInstance()));
        response.put("TxPowProcesssor",getInfo(main()->getTxPoWProcessor()));
        response.put("TxPowMiner",getInfo(main()->getTxPoWMiner()));
        response.put("NIOManager",getInfo(main()->getNIOManager()));
        response.put("P2PManager",getInfo(sqlRequired(main()->getNetworkManager()).getP2PManager()));
        response.put("SendPollManager",getInfo(main()->getSendPoll()));
        response.put("NotifyManager",getInfo(main()->getNotifyManager()));
        auto timers=TimerProcessor::getTimerProcessor();response.put("TimerProcessor",sqlRequired(timers).getSize());
        response.put("RWLockInfo",sqlRequired(MinimaDB::getDB()).getRWLockInfo());
        response.put("writelockthread",sqlRequired(MinimaDB::getDB()).currentWriteLockThread());
        response.put("writelockthreadstate",sqlRequired(MinimaDB::getDB()).mCurrentWriteLockState.load());
        response.put("Shutting Down",main()->isShuttingDown());
        MinimaLogger::log("Posting Checker Call in TxPoWProcessor..");
        sqlRequired(main()->getTxPoWProcessor()).postCheckCall();
        MinimaLogger::log("Posting Checker Call in Main..");main()->PostMessage(Main::MAIN_CALLCHECKER);
        auto message=std::make_shared<Message>(Main::MAIN_CALLCHECKER);message->addBoolean("timer",true);
        auto timer=std::make_shared<TimerMessage>(1000,message);
        MinimaLogger::log("Posting TIMED Checker Call in Main..");main()->PostTimerMessage(timer);
    }else if(action=="details"){
        auto processor=getParam("processor");
        if(javaStringEqualsIgnoreCase(processor,"p2pmanager"))printDetails(sqlRequired(main()->getNetworkManager()).getP2PManager());
        else if(javaStringEqualsIgnoreCase(processor,"niomanager"))printDetails(main()->getNIOManager());
        else if(javaStringEqualsIgnoreCase(processor,"main"))printDetails(Main::getInstance());
        else if(javaStringEqualsIgnoreCase(processor,"txpowprocessor"))printDetails(main()->getTxPoWProcessor());
        else if(javaStringEqualsIgnoreCase(processor,"txpowminer"))printDetails(main()->getTxPoWMiner());
        else if(javaStringEqualsIgnoreCase(processor,"notifymanager"))printDetails(main()->getNotifyManager());
        else if(javaStringEqualsIgnoreCase(processor,"senpollmanager"))printDetails(main()->getSendPoll());
        else if(javaStringEqualsIgnoreCase(processor,"timerprocessor")){
            MinimaLogger::log("Processor Details  : TimerProcessor",false);
            sqlRequired(TimerProcessor::getTimerProcessor()).printAllMessages();
        }
        response.put("details","Sent to Minima Log");
    }
    result.put("response",response);return result;
}
void systemcheck::printDetails(const std::shared_ptr<MessageProcessor>& processor){
    MinimaLogger::log("Processor Details : "+sqlRequired(processor).getName(),false);
    processor->printAllMessages();
}
json::JSONObject systemcheck::getInfo(const std::shared_ptr<MessageProcessor>& processor){
    json::JSONObject result;result.put("stack",sqlRequired(processor).getSize());
    auto last=processor->getLastMessage();result.put("lastmessage",last?json::JSONValue(last->toString()):json::JSONValue());return result;
}
}
