#include "support/FixedTaskExecutor.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <future>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace {
struct Context {
    std::mutex lock;JSONArray events;std::string kind;std::thread::id original;
    std::promise<void> first,releaseTask,second,handling,releaseHandler,handled;
    void record(const std::string& text){std::lock_guard guard(lock);events.add(text);}
};
struct Handler:NativeUncaughtExceptionHandler {
    std::function<void(const NativeThread*,const std::shared_ptr<MinimaLogger::Throwable>&)> callback;
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& error)override{callback(thread,error);}
};
void await(std::shared_future<void> value){if(value.wait_for(std::chrono::seconds(5))!=std::future_status::ready)throw std::runtime_error("Fixture timeout");value.get();}
}
int main(int argc,char** argv)try{
    if(argc!=3)return 2;auto s=std::make_shared<Context>();s->kind=argv[1];bool queued=std::string(argv[2])=="true";
    auto first=s->first.get_future().share(),releaseTask=s->releaseTask.get_future().share(),second=s->second.get_future().share(),handling=s->handling.get_future().share(),releaseHandler=s->releaseHandler.get_future().share(),handled=s->handled.get_future().share();
    auto stale=std::make_shared<Handler>();stale->callback=[s](auto,auto){s->record("STALE-HANDLER");};setDefaultUncaughtExceptionHandler(stale);FixedTaskExecutor pool(1);
    pool.execute([s,releaseTask]{s->original=std::this_thread::get_id();s->record("first");s->first.set_value();await(releaseTask);if(s->kind=="error")throw JavaError("java.lang.LinkageError","task");if(s->kind=="oom")throw JavaOutOfMemoryError("task");if(s->kind=="io")throw wire::IOException("task");throw JavaTypedException("java.lang.IllegalStateException","task");});await(first);
    if(queued)pool.execute([s]{s->record(std::string("replacement:")+(std::this_thread::get_id()!=s->original?"true":"false"));s->second.set_value();});else s->second.set_value();
    auto handler=std::make_shared<Handler>();handler->callback=[s,second,releaseHandler](auto thread,auto error){await(second);s->record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(error->toString().value_or(u"null")));s->handling.set_value();await(releaseHandler);s->record("handler-return");s->handled.set_value();if(s->kind=="defaultthrows")throw std::runtime_error("default");};setDefaultUncaughtExceptionHandler(handler);
    pool.shutdown();s->record(std::string("early-termination:")+(pool.awaitTermination(0)?"true":"false"));s->releaseTask.set_value();await(handling);s->record(std::string("terminated-during-handler:")+(pool.awaitTermination(5000)?"true":"false"));s->releaseHandler.set_value();await(handled);setDefaultUncaughtExceptionHandler(nullptr);JSONObject result;result.put("events",s->events);std::cout<<result.toJSONString()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
