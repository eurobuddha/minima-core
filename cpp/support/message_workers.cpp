#include "org/minima/utils/messages/MessageProcessor.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include <iostream>
#include <latch>
#include <future>
using namespace minima;
static void require(bool condition,const char* what){if(!condition)throw std::runtime_error(what);}
class Worker : public MessageProcessor {
    std::function<void(std::shared_ptr<Message>)> mHandler;
    Worker(std::function<void(std::shared_ptr<Message>)> handler,std::function<void()> release):MessageProcessor("test",std::move(release)),mHandler(std::move(handler)){}
    void processMessage(std::shared_ptr<Message> message) override {mHandler(std::move(message));}
public:
    static auto create(std::function<void(std::shared_ptr<Message>)> handler,std::function<void()> release){return std::shared_ptr<Worker>(new Worker(std::move(handler),std::move(release)));}
    void start(){startMessageProcessor();}
};
int main()try{
    std::atomic<int> count{0},released{0};std::array<int,4> last{-1,-1,-1,-1};std::atomic<bool> ordered{true};std::latch done(2000);
    auto worker=Worker::create([&](auto m){auto p=m->getInteger("producer"),n=m->getInteger("n");if(last.at(p)+1!=n)ordered=false;last.at(p)=n;++count;if(n%199==0)throw std::runtime_error("expected handler failure");},[&]{++released;done.count_down();});worker->start();
    std::vector<std::thread> producers;for(int p=0;p<4;++p)producers.emplace_back([&,p]{for(int n=0;n<500;++n){auto m=std::make_shared<Message>("DATA");m->addInteger("producer",p).addInteger("n",n);worker->PostMessage(m);}});for(auto& p:producers)p.join();done.wait();worker->stopMessageProcessor();worker->joinMessageProcessor();require(ordered&&count==2000&&released==2000,"FIFO or finally cleanup failed");require(worker->isShutdownComplete()&&!worker->isRunning(),"Shutdown failed");
    int stoppedCalls=0;std::weak_ptr<Worker> stopTarget;auto selfStop=Worker::create([&](auto){++stoppedCalls;stopTarget.lock()->stopMessageProcessor();},[]{});stopTarget=selfStop;for(const auto* type:{"A","B","C"})selfStop->PostMessage(type);selfStop->start();selfStop->joinMessageProcessor();require(stoppedCalls==1&&selfStop->getSize()==1,"Stop must discard the already-fetched next message");
    auto queue=Worker::create([](auto){},[]{});auto manual=TimerProcessor::createManual();
    auto a=std::make_shared<TimerMessage>(10,"A",1000),b=std::make_shared<TimerMessage>(-10,"B",1000),c=std::make_shared<TimerMessage>(0,"C",1000);for(auto t:{a,b,c}){t->setProcessor(queue);manual->PostMessage(t);}manual->PostMessage(nullptr);
    manual->pollAt(1000);require(queue->getSize()==1&&manual->getSize()==2,"Timer must use strict less-than deadline");manual->pollAt(1011);require(queue->getSize()==3&&manual->getSize()==0,"Timer delivery failed");
    queue->stopMessageProcessor();auto pending=std::make_shared<TimerMessage>(0,"STOPPED",0);pending->setProcessor(queue);manual->PostMessage(pending);manual->pollAt(1);require(queue->getSize()==3&&manual->getSize()==0,"Stopped receiver accepted timer");
    std::promise<void> entered,finish;auto finishFuture=finish.get_future().share();auto blocked=Worker::create([&](auto){entered.set_value();finishFuture.wait();},[]{});blocked->start();blocked->PostMessage("WAIT");entered.get_future().wait();blocked->waitToShutDown(0);require(blocked->isShutdownComplete()&&!blocked->isRunning(),"Hard-shutdown flag mismatch");auto weak=std::weak_ptr(blocked);blocked.reset();require(!weak.expired(),"Worker ownership lost while callback active");finish.set_value();if(auto held=weak.lock())held->joinMessageProcessor();for(int i=0;i<100&&!weak.expired();++i)std::this_thread::sleep_for(std::chrono::milliseconds(2));require(weak.expired(),"Worker leaked after callback exit");
    std::latch realDone(1);auto real=Worker::create([&](auto m){require(m->isMessageType("TIMER"),"Unexpected timer");realDone.count_down();},[]{});real->start();TimerProcessor::createTimerProcessor();real->PostTimerMessage(std::make_shared<TimerMessage>(0,"TIMER"));realDone.wait();real->stopMessageProcessor();real->joinMessageProcessor();TimerProcessor::stopTimerProcessor();TimerProcessor::getTimerProcessor()->join();
    selfStop->stopMessageProcessor();manual->stop();
    MessageProcessor::setTrace(true,"aΣ1i");MessageProcessor::setTraceLanguage("el");require(real->checkTraceFilter("AΣ1I"),"Trace lowercase failed");MessageProcessor::setTrace(false,"");
    std::cout<<"2000 concurrent messages, FIFO, exceptions and lock cleanup; strict/manual/real timers; worker lifetime and shutdown passed\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
