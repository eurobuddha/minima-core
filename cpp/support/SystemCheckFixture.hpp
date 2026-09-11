#pragma once
#include "MainFixture.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/commands/base/systemcheck.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
namespace minima {
class SystemCheckFixture {
    class Processor:public TxPoWProcessor {public:Processor():TxPoWProcessor([]{TxPoWProcessorServices s;s.releaseLocks=[]{};s.isShuttingDown=s.isShuttingDownOrRestoring=[]{return false;};s.postMain=[](auto){};s.postNotifyEvent=[](const auto&,const auto&){};s.setSyncIBD=[](bool){};return s;}()){} };
    class Miner:public TxPoWMiner {public:Miner():TxPoWMiner([]{TxPoWMinerServices s;s.releaseLocks=[]{};s.postMain=[](auto){};s.automineTimer=[]{return int64_t(50000);};return s;}()){} };
    class Poll:public SendPollManager {public:Poll():SendPollManager([](const auto& text){return CommandRunner::getRunner().runSingleCommand(text.value());},[] {}){} };
    class Notify:public NotifyManager {public:Notify():NotifyManager([]{NotifyManagerServices s;s.releaseLocks=[]{};s.getListener=Main::getMinimaListener;return s;}()){} };
public:
    static std::vector<std::shared_ptr<MessageProcessor>> processors(const std::shared_ptr<Main>& main){
        auto net=main->getNetworkManager();return {main,main->getTxPoWProcessor(),main->getTxPoWMiner(),net?net->getNIOManager():nullptr,net?net->getP2PManager():nullptr,main->getSendPoll(),main->getNotifyManager()};
    }
    static void setup(const std::shared_ptr<Main>& main,const json::JSONObject& input){
        auto missing=input.getString("missing","");
        MainFixture::diagnosticProcessors(*main,missing=="processor"?nullptr:std::make_shared<Processor>(),missing=="miner"?nullptr:std::make_shared<Miner>(),missing=="poll"?nullptr:std::make_shared<Poll>(),missing=="notify"?nullptr:std::make_shared<Notify>());
        int index=0;for(auto& proc:processors(main)){
            if(proc){proc->clear();{std::lock_guard lock(proc->mLock);proc->mLastMessage=input.containsKey("last")&&!input.get("last").isNull()?std::make_shared<Message>(input.getString("last")):nullptr;}
                if(input.containsKey("queued")&&input.getBoolean("queued"))for(int i=0;i<=index;++i)proc->PostMessage(std::make_shared<Message>("QUEUED_"+std::to_string(i)));
                if(input.containsKey("nullQueue")&&input.getBoolean("nullQueue"))proc->PostMessage(std::shared_ptr<Message>());
            }++index;
        }
        if(!input.containsKey("skipTimer")||!input.getBoolean("skipTimer")){
            TimerProcessor::createTimerProcessor();auto timer=TimerProcessor::getTimerProcessor();timer->stop();timer->join();
            if(input.containsKey("queued")&&input.getBoolean("queued"))timer->PostMessage(std::make_shared<TimerMessage>(100000,"FIXTURE_TIMER",1000));
        }
        MinimaDB::getDB()->writeLock(true);MinimaDB::getDB()->writeLock(false);
        if(missing=="main")MainFixture::install(nullptr);
    }
    static json::JSONArray snapshot(const std::shared_ptr<Main>& main){
        json::JSONArray all;for(const auto& proc:processors(main)){if(!proc){all.add(json::JSONValue());continue;}json::JSONArray queue;std::lock_guard lock(proc->mLock);for(const auto& message:proc->mMessages){if(!message){queue.add(json::JSONValue());continue;}json::JSONObject row;row.put("type",message->getMessageType()?json::JSONValue(*message->getMessageType()):json::JSONValue());row.put("timer",message->getBoolean("timer",false));if(message->isMessageType("NOTIFY_POST")){auto notify=message->getObject("notify").as<std::shared_ptr<json::JSONObject>>();auto data=std::get<std::shared_ptr<json::JSONObject>>(notify->get("data").value);auto text=data->getString("message","");auto pos=text.find("] : ");row.put("stackFrame",pos!=std::string::npos&&text.substr(pos+4).starts_with("     "));}queue.add(row);}all.add(queue);}return all;
    }
};
}
