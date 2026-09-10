#include "org/minima/system/commands/sendpoll/SendPollManager.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include <iostream>
#include <future>
#include <set>
using namespace minima;
static void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
class Poll : public SendPollManager {
    Poll(std::function<json::JSONObject(const std::optional<std::string>&)> run):SendPollManager(std::move(run),[] {}){}
    void pauseBetweenCommands() override {++pauses;if(onPause)onPause();}
    void schedulePoll() override {++scheduled;}
public:
    int pauses=0,scheduled=0;
    std::function<void()> onPause;
    static auto create(std::function<json::JSONObject(const std::optional<std::string>&)> run){return std::shared_ptr<Poll>(new Poll(std::move(run)));}
    void poll(const Message::Key& type=SENDPOLL_FUNCTION){processMessage(std::make_shared<Message>(type));}
};
int main()try{
    SendPollMessage nulls(std::nullopt,std::nullopt);require(nulls.toJSON().toJSONString()=="{\"uid\":null,\"command\":null}","Null JSON fields");
    SendPollMessage escaped("id",std::string("send \"x\"\nπ"));require(escaped.copy().toJSON().toJSONString()==escaped.toJSON().toJSONString(),"Message copy");
    std::set<std::string> ids;for(int i=0;i<1000;++i){SendPollMessage m("send");require(MiniData::fromHex(*m.getUID()).getLength()==16,"Random UID width");ids.insert(*m.getUID());}require(ids.size()==1000,"Repeated random UID");
    std::vector<std::string> called;std::weak_ptr<Poll> target;
    auto manager=Poll::create([&](const auto& command){called.push_back(command.value_or("NULL"));if(command=="mutate")target.lock()->addSendCommand("new");json::JSONObject result;result.put("status",command!="retry");return result;});target=manager;
    for(const auto* c:{"done","retry","mutate"})manager->addSendCommand(c);
    auto snapshot=manager->listCommands();manager->removeCommand(std::nullopt);require(manager->listCommands().size()==3,"Null remove matched UID");
    manager->poll("OTHER");require(called.empty()&&manager->scheduled==0,"Unknown event polled");manager->poll();
    require(called==std::vector<std::string>({"done","retry","mutate"}),"Snapshot iteration");auto remaining=manager->listCommands();require(remaining.size()==2&&remaining[0].getCommand()=="retry"&&remaining[1].getCommand()=="new","Successful removal or concurrent insertion");
    require(snapshot.size()==3&&snapshot[0].getCommand()=="done","List copy alias");require(manager->pauses==3&&manager->scheduled==1,"Poll cadence");manager->removeCommand(remaining[0].getUID());require(manager->listCommands().size()==1,"UID removal");
    int failures=0;auto broken=Poll::create([&](const auto&){++failures;return json::JSONObject();});broken->addSendCommand("invalid status");try{broken->poll();throw std::runtime_error("Missing status accepted");}catch(const std::bad_variant_access&){}require(failures==1&&broken->pauses==0&&broken->scheduled==0&&broken->listCommands().size()==1,"Exception must abort scheduling");
    try{manager->poll(std::nullopt);throw std::runtime_error("Null type accepted");}catch(const std::invalid_argument&){}
    auto stopped=Poll::create([](const auto&){json::JSONObject result;result.put("status",false);return result;});stopped->addSendCommand("one");stopped->addSendCommand("two");stopped->onPause=[&]{stopped->stopMessageProcessor();};stopped->poll();require(stopped->pauses==1&&stopped->scheduled==1&&stopped->listCommands().size()==2,"Stop during poll");
    auto concurrent=Poll::create([](const auto&){return json::JSONObject();});std::vector<std::thread> producers;for(int p=0;p<4;++p)producers.emplace_back([&]{for(int i=0;i<250;++i)concurrent->addSendCommand("queued");});for(auto& p:producers)p.join();auto rows=concurrent->listCommands();require(rows.size()==1000,"Concurrent adds lost");producers.clear();for(int p=0;p<4;++p)producers.emplace_back([&,p]{for(int i=p*250;i<(p+1)*250;++i)concurrent->removeCommand(rows[i].getUID());});for(auto& p:producers)p.join();require(concurrent->listCommands().empty(),"Concurrent removals lost");
    TimerProcessor::createTimerProcessor();std::promise<void> ran;std::atomic<int> releases=0;auto real=SendPollManager::create([&](const auto& command){require(command=="offline","Wrong command");json::JSONObject result;result.put("status",true);ran.set_value();return result;},[&]{++releases;});real->addSendCommand("offline");real->PostMessage(SendPollManager::SENDPOLL_FUNCTION);require(ran.get_future().wait_for(std::chrono::seconds(10))==std::future_status::ready,"Real worker failed");real->stopMessageProcessor();real->joinMessageProcessor();require(real->listCommands().empty()&&releases==1,"Real worker cleanup");TimerProcessor::stopTimerProcessor();TimerProcessor::getTimerProcessor()->join();
    manager->stopMessageProcessor();broken->stopMessageProcessor();concurrent->stopMessageProcessor();
    std::cout<<"Send polling: copied snapshots, success/retry/removal, reentrant insertion, exceptions, stop, 1000 concurrent adds/removals, real worker/timer cleanup passed\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
