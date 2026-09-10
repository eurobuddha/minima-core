#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/system/network/webhooks/NotifyManager.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
#include <latch>
#include <set>
#include <sstream>
using namespace minima;
namespace {
void require(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
struct Receiver:MinimaLogger::MainReceiver {
    std::function<void(const std::string&,std::shared_ptr<json::JSONObject>)> callback;
    void PostNotifyEvent(const std::string& event,std::shared_ptr<json::JSONObject> data)override{callback(event,std::move(data));}
};
struct Listener:MessageListener {
    std::function<void(std::shared_ptr<Message>)> callback;
    void processMessage(std::shared_ptr<Message> message)override{callback(std::move(message));}
};
struct FinalLog {
    int& destroyed;
    explicit FinalLog(int& value):destroyed(value){}
    ~FinalLog(){++destroyed;MinimaLogger::log("released-runtime",false);}
};
MinimaLogger::Runtime fixed(){MinimaLogger::Runtime r;r.totalMemory=[]{return int64_t(2048);};r.freeMemory=[]{return int64_t(1024);};r.currentTimeMillis=[]{return int64_t(1700000000000);};return r;}
}
int main()try{
    std::mutex lock;std::set<std::u16string> printed,posted;std::atomic<int> lookups{0};auto receiver=std::make_shared<Receiver>();
    receiver->callback=[&](const auto& event,auto data){std::lock_guard guard(lock);auto text=data->get("message").asUTF16();require(event=="MINIMALOG","Wrong event name");require(printed.contains(text),"Notification preceded printing");require(posted.insert(text).second,"Duplicate notification");};
    auto r=fixed();r.getMain=[&]{++lookups;return receiver;};r.println=[&](const auto& text){std::lock_guard guard(lock);require(printed.insert(text).second,"Duplicate output");};MinimaLogger::setRuntime(r);
    std::vector<std::thread> workers;for(int thread=0;thread<4;++thread)workers.emplace_back([&,thread]{for(int i=0;i<100;++i)MinimaLogger::log("worker-"+std::to_string(thread)+"-"+std::to_string(i));});for(auto& worker:workers)worker.join();
    require(printed.size()==400&&posted==printed&&lookups==800,"Concurrent log loss or lookup order");
    int destroyed=0;auto retiring=std::make_shared<FinalLog>(destroyed);r=fixed();r.getMain=[retiring]{return std::shared_ptr<MinimaLogger::MainReceiver>{};};MinimaLogger::setRuntime(r);r={};retiring.reset();
    bool releasePrinted=false;r=fixed();r.println=[&](const auto& text){require(unicode::encodeUTF8(text).ends_with(" : released-runtime"),"Wrong retirement log");releasePrinted=true;};MinimaLogger::setRuntime(r);require(destroyed==1&&releasePrinted,"Retired receiver did not reenter logging");
    // The source resolves Main twice, after printing. A disappearing owner
    // must throw at the second lookup rather than retain the first receiver.
    bool emitted=false;r=fixed();r.println=[&](const auto&){emitted=true;};int reads=0;r.getMain=[&]{return ++reads==1?receiver:nullptr;};MinimaLogger::setRuntime(r);bool threw=false;try{MinimaLogger::log("rebind");}catch(const JavaNullPointerException&){threw=true;}require(emitted&&threw&&reads==2,"Main lookup rebinding changed");
    std::vector<std::string> order;r=fixed();r.totalMemory=[&]{order.push_back("total");return INT64_MIN;};r.freeMemory=[&]{order.push_back("free");return int64_t(1);};r.currentTimeMillis=[&]{order.push_back("time");return int64_t(0);};r.println=[&](const auto& text){order.push_back("print");require(unicode::encodeUTF8(text).find(" [8.0 EB] : wrap")!=std::string::npos,"Heap subtraction did not wrap like Java long");};r.getMain=[&]{throw std::runtime_error("Unexpected Main lookup");return receiver;};MinimaLogger::setRuntime(r);MinimaLogger::log("wrap",false);require(order==std::vector<std::string>{"total","free","time","print"},"Heap/date/print order changed");
    // Default native heap/clock/stdout operate without a Main instance.
    MinimaLogger::setRuntime({});auto native=MinimaLogger::getRuntime();require(native.totalMemory()>=0&&native.freeMemory()>=0,"Invalid native heap sample");std::ostringstream captured;auto prior=std::cout.rdbuf(captured.rdbuf());MinimaLogger::log("native-default",false);std::cout.rdbuf(prior);require(captured.str().starts_with("Minima @ ")&&captured.str().ends_with(" : native-default\n"),"Native runtime defaults did not print");
    // Actual NotifyManager default log -> logger -> Main receiver -> queued
    // MINIMALOG. Only Main's still-unported receiver is controlled here.
    auto db=std::make_shared<UserDB>();auto listener=std::make_shared<Listener>();std::latch done(2);std::weak_ptr<NotifyManager> manager;int delivered=0,output=0;
    listener->callback=[&](auto message){auto data=message->getObject("notify").template as<std::shared_ptr<json::JSONObject>>();auto event=data->getString("event");if(event=="TEST")throw std::runtime_error("expected listener failure");require(event=="MINIMALOG","Unexpected queued log event");auto payload=std::get<std::shared_ptr<json::JSONObject>>(data->get("data").value);require(payload->getString("message").find("expected listener failure")!=std::string::npos,"Lost failure text");++delivered;};
    receiver=std::make_shared<Receiver>();receiver->callback=[&](const auto& name,auto payload){auto owner=manager.lock();require(bool(owner),"Lost notification owner");auto event=std::make_shared<json::JSONObject>();event->put("event",name);json::JSONValue value;value.value=payload;event->put("data",value);owner->PostEvent(event);};
    r=fixed();r.getMain=[receiver]{return receiver;};r.println=[&](const auto&){++output;};MinimaLogger::setRuntime(r);
    NotifyManagerServices services;services.releaseLocks=[&]{done.count_down();};services.getUserDB=[db]{return db;};services.getListener=[listener]{return listener;};
    auto owner=NotifyManager::create(services);manager=owner;auto event=std::make_shared<json::JSONObject>();event->put("event","TEST");owner->PostEvent(event);done.wait();owner->shutDown();owner->joinMessageProcessor();require(delivered==1&&output==1,"Default logger notification integration failed");MinimaLogger::setRuntime({});
    std::cout<<"Logger: 400 concurrent print-before-notify pairs, double Main lookup, heap wrap/order, native defaults and queued listener-error logging PASS\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
