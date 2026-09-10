#include "org/minima/system/network/p2p/P2PPeersChecker.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <condition_variable>
#include <chrono>
#include <iostream>
using namespace minima;using namespace minima::p2p;
namespace minima {class NIOManagerFixture {public:static int64_t delay(const TimerMessage& timer){return timer.mDelay;}};}
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
class Queue : public MessageStack {public:using MessageStack::getNextMessage;};
class Checker : public P2PPeersChecker {
    Checker(const std::shared_ptr<MessageStack>& parent):P2PPeersChecker(parent,[]{return std::shared_ptr<TxPoWTreeNode>{};},[]{}){}
public:
    using MessageStack::getNextMessage;
    std::optional<Greeting> reply;
    std::vector<std::shared_ptr<TimerMessage>> timers;
    std::vector<int32_t> bounds;
    int32_t draw=0;
    static std::shared_ptr<Checker> create(const std::shared_ptr<MessageStack>& parent){return std::shared_ptr<Checker>(new Checker(parent));}
    void process(const std::shared_ptr<Message>& message){processMessage(message);}
    void PostTimerMessage(const std::shared_ptr<TimerMessage>& message) override {timers.push_back(message);}
protected:
    int32_t newRandomInt(int32_t bound) override {bounds.push_back(bound);require(draw>=0&&draw<bound,"Invalid random fixture value");return draw;}
    std::optional<Greeting> ping(const SocketAddressPtr&) override {return reply;}
};
static std::shared_ptr<Message> message(const char* type,const SocketAddressPtr& address,bool force=false){auto m=std::make_shared<Message>(type);m->addObject("address",MessageObject(address));if(force)m->addBoolean("force",true);return m;}
template<class T>static std::shared_ptr<Message> pop(const std::shared_ptr<T>& queue,const char* type){auto value=queue->getNextMessage();require(value&&value->isMessageType(type),"Wrong queued message");return value;}
int main(){
    GeneralParams::TEST_PARAMS=false;GeneralParams::ALLOW_ALL_IP=false;P2PParams::LOG_LEVEL=P2PLogLevel::DEBUG;
    P2PFunctions::enumerateInterfaces=[]{return std::make_shared<JavaStringSet>();};std::vector<std::string> logs;P2PFunctions::logger=[&](const auto& line){logs.push_back(line);};
    auto parent=std::make_shared<Queue>();auto checker=Checker::create(parent);auto address=std::make_shared<InetSocketAddress>("8.8.8.8",9001);auto other=std::make_shared<InetSocketAddress>("8.8.4.4",9001);
    pop(checker,P2PPeersChecker::PEERS_INIT);require(!checker->haveAnyPeers(),"Initial peer sets not empty");
    checker->checkUnverifiedPeer(address);checker->checkUnverifiedPeer(address);require(checker->getUnverifiedPeers()->size()==1,"Admission not deduplicated");pop(checker,P2PPeersChecker::PEERS_CHECKPEERS);require(checker->getSize()==0,"Duplicate admission queued twice");
    checker->process(message(P2PPeersChecker::PEERS_ADDPEERS,std::make_shared<InetSocketAddress>("127.0.0.1",9001)));require(checker->getUnverifiedPeers()->size()==1,"Local peer was admitted");
    Greeting reply;reply.getExtraData().put("50block","1");reply.getExtraData().put("50hash","0x00");checker->reply=reply;
    checker->process(message(P2PPeersChecker::PEERS_CHECKPEERS,address,true));require(checker->getVerifiedPeers()->contains(address)&&checker->getUnverifiedPeers()->isEmpty(),"Peer not promoted");pop(parent,"P2P_ADD_PEER");
    checker->reply.reset();checker->process(message(P2PPeersChecker::PEERS_CHECKPEERS,address,true));require(checker->getVerifiedPeers()->isEmpty()&&checker->getUnverifiedPeers()->contains(address),"Failed peer not demoted");require(checker->timers.size()==1&&NIOManagerFixture::delay(*checker->timers.back())==1800000,"Wrong failed-peer retry");pop(parent,"P2P_REMOVE_PEER");
    checker->process(message(P2PPeersChecker::PEERS_CHECKPEERS,address,true));require(checker->getUnverifiedPeers()->isEmpty()&&checker->timers.size()==1,"Unverified failure retained or retried");pop(parent,"P2P_REMOVE_PEER");
    P2PPeersChecker::MAX_VERIFIED_PEERS=1;checker->getVerifiedPeers()->add(other);checker->draw=89;checker->checkUnverifiedPeer(address);require(checker->getUnverifiedPeers()->isEmpty()&&checker->bounds.back()==100,"90 percent admission rejection failed");checker->draw=90;checker->checkUnverifiedPeer(address);require(checker->getUnverifiedPeers()->contains(address),"10 percent admission branch failed");pop(checker,P2PPeersChecker::PEERS_CHECKPEERS);
    checker->draw=0;checker->reply=reply;checker->process(message(P2PPeersChecker::PEERS_CHECKPEERS,address,true));require(checker->bounds.back()==1,"Eviction bound mismatch");auto removed=pop(parent,"P2P_REMOVE_PEER");require(removed->getObject("address").as<SocketAddressPtr>()==other,"Wrong peer evicted");pop(parent,"P2P_ADD_PEER");require(checker->getVerifiedPeers()->size()==1&&checker->getVerifiedPeers()->contains(address),"Replacement wrong");
    checker->getVerifiedPeers()->clear();checker->getVerifiedPeers()->add(nullptr);checker->process(message(P2PPeersChecker::PEERS_CHECKPEERS,address,true));require(checker->getVerifiedPeers()->size()==2&&checker->getVerifiedPeers()->contains(nullptr),"Source null-selection behavior changed");pop(parent,"P2P_ADD_PEER");require(parent->getSize()==0,"Null selection incorrectly sent removal");
    checker->getVerifiedPeers()->clear();checker->getVerifiedPeers()->add(address);P2PFunctions::openURL=[](const auto&){};
    checker->process(std::make_shared<Message>(P2PPeersChecker::PEERS_FORCEFULLCHECK));auto forced=pop(checker,P2PPeersChecker::PEERS_CHECKPEERS);require(forced->getBoolean("forcelog")&&!forced->exists("force"),"Full check incorrectly forces disconnected ping");
    checker->process(std::make_shared<Message>(P2PPeersChecker::PEERS_LOOP));pop(checker,P2PPeersChecker::PEERS_CHECKPEERS);require(NIOManagerFixture::delay(*checker->timers.back())==21600000,"Wrong loop timer");
    P2PFunctions::openURL=[](const auto&){throw wire::IOException("offline");};auto timerCount=checker->timers.size();checker->process(std::make_shared<Message>(P2PPeersChecker::PEERS_FORCEFULLCHECK));require(checker->timers.size()==timerCount&&checker->getSize()==0,"Offline force check scheduled work");checker->process(std::make_shared<Message>(P2PPeersChecker::PEERS_LOOP));require(checker->timers.size()==timerCount+1,"Offline loop failed to reschedule");
    std::mutex mutex;std::condition_variable changed;int released=0;auto worker=P2PPeersChecker::create(parent,[]{return std::shared_ptr<TxPoWTreeNode>{};},[&]{std::lock_guard lock(mutex);++released;changed.notify_all();});
    {std::unique_lock lock(mutex);require(changed.wait_for(lock,std::chrono::seconds(3),[&]{return released>0;}),"Checker initialization worker stalled");}
    worker->stopMessageProcessor();worker->joinMessageProcessor();require(worker->isShutdownComplete(),"Worker shutdown incomplete");std::weak_ptr<P2PPeersChecker> weak=worker;worker.reset();require(weak.expired(),"Stopped worker ownership leaked");
    P2PFunctions::openURL={};P2PFunctions::logger={};P2PFunctions::enumerateInterfaces={};P2PPeersChecker::MAX_VERIFIED_PEERS=250;
    std::cout<<"Peer admission, promotion, retry, capacity, loop and real worker lifecycle passed\n";
}
