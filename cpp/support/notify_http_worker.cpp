#include "org/minima/system/network/webhooks/NotifyManager.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include <iostream>
#include <latch>
using namespace minima;
using namespace std::string_literals;
namespace {
class Listener:public MessageListener {
public:
    void processMessage(std::shared_ptr<Message> message) override {
        auto event=message->getObject("notify").as<std::shared_ptr<json::JSONObject>>();
        event->put("listener",true);
    }
};
}
int main(int argc,char** argv)try{
    if(argc!=2)throw std::invalid_argument("Expected a loopback origin");
    std::string origin=argv[1];
    if(!origin.starts_with("http://127.0.0.1:"))throw std::invalid_argument("Loopback fixture only");
    std::string first(8192,'a'),second(8192,'b');RPCClient::USER_AGENT=first;
    std::atomic<bool> intact{true};std::vector<std::thread> writers;
    for(int thread=0;thread<4;++thread)writers.emplace_back([&,thread]{for(int i=0;i<1000;++i){RPCClient::USER_AGENT=thread%2?first:second;RPCClient::String value=RPCClient::USER_AGENT;if(!value||(*value!=first&&*value!=second))intact=false;}});
    for(auto& writer:writers)writer.join();RPCClient::USER_AGENT="Minima/1.0";
    if(!intact)throw std::runtime_error("Torn USER_AGENT snapshot");
    auto db=std::make_shared<UserDB>();
    db->setWebHooks({"TEST#"+origin+"/status/200","#"+origin+"/status/404",
        "OTHER#"+origin+"/redirect/307?to=/status/200","#"+origin+"/status/200?last"});
    auto listener=std::make_shared<Listener>();std::latch done(2);json::JSONArray logs;
    NotifyManagerServices services;
    services.releaseLocks=[&]{done.count_down();};
    services.getUserDB=[db]{return db;};services.getListener=[listener]{return listener;};
    services.log=[&](const auto& text,bool notify){json::JSONObject row;row.put("text",text);row.put("notify",notify);logs.add(row);};
    // Deliberately omit sendPOST: the actual production default must deliver.
    auto manager=NotifyManager::create(std::move(services));manager->WEBHOOKS_ERROR_LOGS=true;
    for(const char* name:{"TEST","OTHER"}){auto event=std::make_shared<json::JSONObject>();event->put("event",name);event->put("body","π😄\r\n\0end"s);manager->PostEvent(event);}
    done.wait();manager->shutDown();manager->joinMessageProcessor();
    if(logs.size()!=2)throw std::runtime_error("Expected one failed delivery per event");
    std::cout<<logs.toJSONString()<<'\n';
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
