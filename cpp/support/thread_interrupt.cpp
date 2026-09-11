#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/system/commands/txn/txnlock.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/commands/sendpoll/SendPollManager.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <future>
#include <iostream>
using namespace minima;using namespace minima::json;using commands::txnlock;
namespace {
std::string sleepResult(int64_t delay){try{nativeThreadSleep(delay);return "returned";}catch(const JavaTypedException& e){return e.className();}catch(const JavaIllegalArgumentException&){return "java.lang.IllegalArgumentException";}}
class Worker:public MessageProcessor {
    std::string mode;
    Worker(std::string value,int& cleanup):MessageProcessor("interrupt-worker",[&]{++cleanup;}),mode(std::move(value)){}
    void processMessage(std::shared_ptr<Message>)override{
        entered.set_value();
        try{
            if(mode=="hard_lock")txnlock().getLock(0);
            else if(mode=="hard_unlock"){
                txnlock command;command.getParams()->put("action","unlock");command.getParams()->put("unlockdelay","9223372036854775807");command.runCommand();
            }else nativeThreadSleep(mode=="soft_sleep"?500:INT64_MAX);
            returned=true;
        }catch(const JavaTypedException& e){error=e.className();pending=nativeThreadInterrupted();throw;}
    }
public:
    std::promise<void> entered;std::string error="none";bool pending=false,returned=false;
    static auto create(std::string mode,int& cleanup){auto p=std::shared_ptr<Worker>(new Worker(std::move(mode),cleanup));p->startMessageProcessor();return p;}
};
}
int main(int argc,char** argv)try{
    if(argc!=2)return 2;std::string mode=argv[1];JSONObject result;result.put("mode",mode);
    MinimaLogger::Runtime log;log.println=[](auto&){};MinimaLogger::setRuntime(log);
    if(mode=="flags"){
        NativeThread descriptor{u"flags"};runNativeThread(descriptor,[&]{
        JSONArray rows;
        for(auto delay:{int64_t(-1),INT64_MIN,int64_t(0),int64_t(1),INT64_MAX}){
            interruptCurrentNativeThread();interruptCurrentNativeThread();JSONObject row;row.put("delay",std::to_string(delay));row.put("error",sleepResult(delay));
            row.put("pending",descriptor.isInterrupted());row.put("consume",nativeThreadInterrupted());row.put("again",nativeThreadInterrupted());rows.add(row);
        }
        result.put("rows",rows);result.put("zero",sleepResult(0));result.put("positive",sleepResult(2));
        });
    }else if(mode=="external"){
        NativeThread descriptor{u"external"};std::promise<void> entered,done;auto finished=done.get_future();std::string error="none";bool pending=true;
        std::thread thread([&]{descriptor.id=std::this_thread::get_id();runNativeThread(descriptor,[&]{entered.set_value();error=sleepResult(INT64_MAX);pending=descriptor.isInterrupted();done.set_value();});});
        entered.get_future().wait();result.put("blocked",finished.wait_for(std::chrono::milliseconds(25))==std::future_status::timeout);descriptor.interrupt();thread.join();result.put("error",error);result.put("pending",pending);
    }else if(mode=="lock_flags"){
        NativeThread descriptor{u"flags"};runNativeThread(descriptor,[&]{
            txnlock::lockFunction(false);interruptCurrentNativeThread();result.put("acquired",txnlock().getLock(-1));result.put("pending_after_acquire",descriptor.isInterrupted());
            txnlock command;command.getParams()->put("action","unlock");command.getParams()->put("unlockdelay","0");command.runCommand();
            result.put("locked",txnlock::mLocked.load());result.put("pending_after_unlock",nativeThreadInterrupted());
        });
    }else if(mode=="hard_poll"){
        int cleanup=0;TimerProcessor::createTimerProcessor();auto timers=TimerProcessor::getTimerProcessor();
        auto worker=SendPollManager::create([](const auto& command){return CommandRunner::getRunner().runSingleCommand(command.value());},[&]{++cleanup;});
        worker->addSendCommand("hash data:interrupt-fixture");worker->PostMessage(SendPollManager::SENDPOLL_FUNCTION);
        auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(3);
        while(!worker->listCommands().empty()&&std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(std::chrono::milliseconds(1));
        worker->waitToShutDown(0);worker->joinMessageProcessor();timers->stop();timers->join();
        result.put("remaining",int32_t(worker->listCommands().size()));result.put("timers",timers->getSize());result.put("cleanup",cleanup);
        result.put("running",worker->isRunning());result.put("shutdown",worker->isShutdownComplete());
    }else{
        int cleanup=0;txnlock::lockFunction(true);auto worker=Worker::create(mode,cleanup);worker->PostMessage("TEST");worker->entered.get_future().wait();
        if(mode=="soft_sleep")worker->stopMessageProcessor();
        else{if(mode=="interrupted_waiter")interruptCurrentNativeThread();worker->waitToShutDown(0);}
        worker->joinMessageProcessor();result.put("error",worker->error);result.put("pending",worker->pending);result.put("returned",worker->returned);
        result.put("cleanup",cleanup);result.put("locked",txnlock::mLocked.load());result.put("running",worker->isRunning());result.put("shutdown",worker->isShutdownComplete());
        result.put("waiter_pending",nativeThreadInterrupted());txnlock::lockFunction(false);
    }
    MinimaLogger::setRuntime({});std::cout<<result.toJSONString()<<'\n';
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
