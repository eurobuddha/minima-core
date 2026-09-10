#include "org/minima/system/network/webhooks/NotifyManager.hpp"
#include "support/ByteIO.hpp"
#include <future>
#include <latch>
#include <iostream>
#include <set>
using namespace minima;
namespace {
void require(bool test,const char* why){if(!test)throw std::runtime_error(why);}
class Listener:public MessageListener {
public:std::function<void(std::shared_ptr<Message>)> call;
    void processMessage(std::shared_ptr<Message> message)override{call(std::move(message));}
};
std::shared_ptr<json::JSONObject> event(int value){auto result=std::make_shared<json::JSONObject>();result->put("event","TEST");result->put("value",value);return result;}
NotifyManagerServices services(const std::shared_ptr<UserDB>& db,const std::shared_ptr<Listener>& listener){
    NotifyManagerServices s;s.releaseLocks=[]{};s.getUserDB=[db]{return db;};s.getListener=[listener]{return listener;};s.sendPOST=[](const auto&,const auto&){return std::string();};s.log=[](const auto&,bool){};return s;
}
}
int main()try{
    auto db=std::make_shared<UserDB>();db->setWebHooks({"#http://fixture/base"});auto listener=std::make_shared<Listener>();std::set<int64_t> seen;int posts=0,cleaned=0;std::latch done(401);
    listener->call=[&](auto message){auto value=message->getObject("notify").template as<std::shared_ptr<json::JSONObject>>();require(seen.insert(std::get<int64_t>(value->get("value").value)).second,"Duplicate delivery");};
    auto s=services(db,listener);s.releaseLocks=[&]{++cleaned;done.count_down();};s.sendPOST=[&](const auto& url,const auto& body){require(url.starts_with("http://fixture/"),"Corrupt hook snapshot");auto decoded=json::JSONValue::parseWithException(body);require(std::get<std::shared_ptr<json::JSONObject>>(decoded.value)->getString("event")=="TEST","Corrupt payload");if(++posts%5==0)throw wire::IOException("expected fixture delivery failure");return std::string();};
    auto manager=NotifyManager::create(s);manager->PostEvent(nullptr);std::vector<std::thread> producers;
    for(int producer=0;producer<4;++producer)producers.emplace_back([&,producer]{for(int index=0;index<100;++index){auto hook="#http://fixture/"+std::to_string(producer);manager->addHook(hook);manager->PostEvent(event(producer*100+index));manager->removeHook(hook);}});
    for(auto& thread:producers)thread.join();done.wait();manager->shutDown();manager->joinMessageProcessor();require(cleaned==401&&seen.size()==400&&posts>=400,"Concurrent notification/recovery/cleanup failed");require(manager->getAllWebHooks()==MiniUtil::UTF16StringList{u"#http://fixture/base"},"Concurrent hooks corrupted");
    // Listener blocks while the outside owner is released. Stop is observed only
    // after the listener returns; the same queued JSONObject stays alive.
    std::promise<void> entered,resume;auto future=resume.get_future().share();std::latch completed(1);int blockedPosts=0;auto payload=event(77);std::weak_ptr<json::JSONObject> weakPayload=payload;
    listener=std::make_shared<Listener>();listener->call=[&](auto message){require(message->getObject("notify").template as<std::shared_ptr<json::JSONObject>>()==weakPayload.lock(),"Lost queued JSON identity");entered.set_value();future.wait();};s=services(db,listener);s.releaseLocks=[&]{completed.count_down();};s.sendPOST=[&](const auto&,const auto&){++blockedPosts;return std::string();};
    auto blocked=NotifyManager::create(s);std::weak_ptr<NotifyManager> weak=blocked;blocked->PostEvent(payload);entered.get_future().wait();blocked->shutDown();blocked.reset();payload.reset();require(!weak.expired()&&!weakPayload.expired(),"Lost active notification owners");resume.set_value();completed.wait();if(auto held=weak.lock())held->joinMessageProcessor();for(int i=0;i<100&&(!weak.expired()||!weakPayload.expired());++i)std::this_thread::sleep_for(std::chrono::milliseconds(2));require(weak.expired(),"Active worker owner leaked");require(weakPayload.expired(),"Queued JSON owner leaked");require(blockedPosts==0,"Stopped worker posted a webhook");
    // The Java monitor is reentrant. Database lookup while adding can inspect
    // the hook list on the same thread; it must already contain the new hook.
    listener=std::make_shared<Listener>();listener->call=[](auto){};std::weak_ptr<NotifyManager> reentrant;s=services(db,listener);bool inspected=false;s.getUserDB=[&]{if(auto current=reentrant.lock()){require(current->getAllWebHooks().size()==2,"Hook mutation occurred after persistence lookup");inspected=true;}return db;};auto owner=NotifyManager::create(s);reentrant=owner;owner->addHook("NEW#http://fixture/reentrant");owner->shutDown();owner->joinMessageProcessor();require(inspected,"Reentrant database lookup missing");
    std::cout<<"Notify worker: 400 concurrent events, malformed-message recovery, hook mutation, cleanup, stop/JSON ownership and reentrant persistence PASS\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
