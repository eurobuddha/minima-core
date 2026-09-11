#include "support/FixedTaskExecutor.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <future>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {
struct FixedTaskExecutorFixture {
    static bool closed(FixedTaskExecutor& pool){std::lock_guard lock(pool.mState->lock);return pool.mState->shutdown;}
    // Mirror shutdown's empty-pool transition while retaining the pool lock.
    // This lets the test deliver an interrupt before the waiter can reacquire it.
    static void signalBeforeInterrupt(FixedTaskExecutor& pool,const NativeThread& waiter){
        std::lock_guard lock(pool.mState->lock);pool.mState->shutdown=true;pool.mState->signalTermination();waiter.interrupt();
    }
    static void interruptAgainDuringReacquire(FixedTaskExecutor& pool,const NativeThread& waiter){
        std::lock_guard lock(pool.mState->lock);waiter.interrupt();auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);
        while(waiter.isInterrupted()){if(std::chrono::steady_clock::now()>end)throw std::runtime_error("first interrupt not consumed before reacquisition");std::this_thread::yield();}
        waiter.interrupt();
    }
};
class NIOManagerFixture:public NIOManager {
    NIOManagerFixture():NIOManager([]{NIOManagerServices s;s.releaseLocks=[]{};s.isShuttingDown=s.isRestoring=[]{return false;};return s;}()){}
public:
    static auto create(){auto value=std::shared_ptr<NIOManagerFixture>(new NIOManagerFixture);value->mNIOServer=std::make_shared<NIOServer>(0,[](auto){});return value;}
    FixedTaskExecutor& pool(){return mThreadPool;}
    void dispatch(){processMessage(std::make_shared<Message>(NIO_SHUTDOWN));}
    bool serverStopped(){return mNIOServer->mShutDown.load();}
};
}
static void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
template<class T>static T ready(std::shared_future<T> future){require(future.wait_for(std::chrono::seconds(5))==std::future_status::ready,"fixture future timeout");return future.get();}
struct HeldTask {
    std::shared_ptr<std::promise<void>> started=std::make_shared<std::promise<void>>();std::promise<void> release;std::shared_future<void> began=started->get_future().share(),gate=release.get_future().share();
    void start(FixedTaskExecutor& pool){pool.execute([signal=started,wait=gate]{signal->set_value();wait.wait();});ready(began);}
};
static JSONObject awaitResult(FixedTaskExecutor& pool,int64_t millis){
    JSONObject result;JSONValue value;std::string error="none";
    try{value=pool.awaitTermination(millis);}catch(const JavaTypedException& e){error=e.toString();}
    result.put("value",value);result.put("error",error);result.put("pending",nativeThreadInterrupted());return result;
}
struct Waiter {
    NativeThread descriptor{u"executor-waiter"};std::promise<JSONObject> done;std::shared_future<JSONObject> result=done.get_future().share();std::thread worker;std::atomic<bool> atFinal{false};
    void start(FixedTaskExecutor& pool,bool reuse=false){worker=std::thread([this,&pool,reuse]{descriptor.id=std::this_thread::get_id();runNativeThread(descriptor,[&]{if(reuse)for(int i=0;i<3;++i)require(!pool.awaitTermination(1),"unexpected early termination");atFinal=true;done.set_value(awaitResult(pool,INT64_MAX));});});}
    void awaitRegistration(){auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);for(;;){if(atFinal){std::lock_guard lock(descriptor.interruption->lock);if(!descriptor.interruption->listener.expired())return;}require(result.wait_for(std::chrono::seconds(0))!=std::future_status::ready,"waiter returned before registration");require(std::chrono::steady_clock::now()<end,"wait registration timeout");std::this_thread::yield();}}
    ~Waiter(){if(worker.joinable()){descriptor.interrupt();worker.join();}}
};
int main(int argc,char** argv)try{
    require(argc>=2,"missing mode");std::string mode=argv[1];JSONObject out;
    if(mode=="matrix"){
        require(argc==5,"matrix args");std::string state=argv[2];auto millis=std::stoll(argv[3]);bool pending=std::string(argv[4])=="true";FixedTaskExecutor pool(1);HeldTask task;
        if(state=="held")task.start(pool);if(state!="open")pool.shutdown();if(pending)interruptCurrentNativeThread();out=awaitResult(pool,millis);
        if(state=="held")task.release.set_value();pool.shutdown();require(pool.awaitTermination(5000),"matrix cleanup did not terminate");
    }else if(mode.starts_with("nio_")){
        bool held=mode.ends_with("held"),hard=mode.starts_with("nio_hard");auto manager=NIOManagerFixture::create();HeldTask task;if(held)task.start(manager->pool());interruptCurrentNativeThread();std::string error="none";
        try{if(hard)manager->hardShutDown();else manager->dispatch();}catch(const JavaTypedException& e){error=e.toString();}
        out.put("error",error);out.put("pending",nativeThreadInterrupted());out.put("poolShutdown",FixedTaskExecutorFixture::closed(manager->pool()));out.put("serverShutdown",manager->serverStopped());out.put("running",manager->isRunning());
        if(held)task.release.set_value();manager->pool().shutdown();require(manager->pool().awaitTermination(5000),"NIO cleanup did not terminate");manager->stopMessageProcessor();
    }else{
        FixedTaskExecutor pool(1);std::vector<std::unique_ptr<Waiter>> waiters;int count=mode=="multiple"?4:1;
        for(int i=0;i<count;++i){auto waiter=std::make_unique<Waiter>();waiter->start(pool,mode=="reuse");waiter->awaitRegistration();waiters.push_back(std::move(waiter));}
        out.put("blocked",true);
        if(mode=="signal_first")FixedTaskExecutorFixture::signalBeforeInterrupt(pool,waiters[0]->descriptor);
        else if(mode=="complete")pool.shutdown();
        else{if(mode=="reinterrupt")FixedTaskExecutorFixture::interruptAgainDuringReacquire(pool,waiters[0]->descriptor);else waiters[0]->descriptor.interrupt();auto first=ready(waiters[0]->result);out.put("beforeShutdown",!FixedTaskExecutorFixture::closed(pool));if(count>1){bool blocked=true;for(int i=1;i<count;++i)blocked&=waiters[i]->result.wait_for(std::chrono::seconds(0))!=std::future_status::ready;out.put("othersBlocked",blocked);}pool.shutdown();}
        JSONArray results;for(auto& waiter:waiters)results.add(ready(waiter->result));out.put("results",results);require(pool.awaitTermination(5000),"cleanup did not terminate");
    }
    std::cout<<out.toJSONString()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
