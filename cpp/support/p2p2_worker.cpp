#include "org/minima/system/network/p2p2/P2P2Manager.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include <semaphore>
#include <chrono>
using namespace minima;using namespace minima::p2p2;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
int main(){
    auto db=std::make_shared<P2P2DB>();auto old=std::make_shared<p2p::P2PDB>();
    p2p::P2PFunctions::logger=[](const auto&){};p2p::P2PFunctions::openURL=[](const auto&){throw wire::IOException("Offline fixture");};
    std::counting_semaphore<8> completed{0};P2P2ManagerServices services;services.releaseLocks=[&]{completed.release();};services.getP2P2DB=[db]{return db;};services.getP2PDB=[old]{return old;};
    TimerProcessor::createTimerProcessor();auto timer=TimerProcessor::getTimerProcessor();auto manager=P2P2Manager::create(services);
    auto await=[&]{return completed.try_acquire_for(std::chrono::seconds(5));};bool initialized=await();
    auto fast=std::make_shared<TimerMessage>(1,P2P2Manager::P2P2_FASTLOOP);manager->PostTimerMessage(fast);bool dispatched=await();
    manager->PostMessage(P2P2Manager::P2P2_SHUTDOWN);bool ignored=await();bool running=manager->isRunning();
    manager->shutdown();manager->joinMessageProcessor();timer->stop();timer->join();
    require(initialized&&dispatched&&ignored,"Actual P2P2 worker/timer stalled");require(running,"Pinned shutdown message should have no branch");require(manager->isShutdownComplete(),"P2P2 worker did not finish");
    require(db->getAllKnownPeers()->size()==2&&db->getAllKnownPeers()->at(0)=="324.45.45.45:9901"&&db->getAllKnownPeers()->at(1)=="324.45.45.45:9902","P2P2 init entries or duplicate behavior changed");require(db->isFirstStartUp(),"Source does not change first-start flag");
    std::weak_ptr<P2P2Manager> weak=manager;manager.reset();require(weak.expired(),"P2P2 timer retained manager");p2p::P2PFunctions::logger={};p2p::P2PFunctions::openURL={};
    std::cout<<"Actual P2P2 initialization, timer dispatch, source no-op message and shutdown ownership passed\n";
}
