#include "PeerRuntimeFixture.hpp"
#include "org/minima/system/network/p2p/P2PManager.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include <latch>
#include <future>
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
int main(){
    constexpr int iterations=2000,readers=4;
    auto state=std::make_shared<P2PState>();state->setMaxNumP2PConnections(0);state->setMaxNumNoneP2PConnections(0);state->setLoopDelay(0);
    auto a=std::make_shared<InetSocketAddress>("127.0.0.1",9001),b=std::make_shared<InetSocketAddress>("127.0.0.2",9001);
    std::atomic<int> errors{0},reads{0};std::latch ready(readers+1),start(1);std::vector<std::thread> threads;
    threads.emplace_back([&]{ready.count_down();start.wait();try{for(int i=0;i<iterations;++i){auto peer=i%2?a:b;auto map=std::make_shared<PeerMap>();map->put("peer",peer);state->setInLinks(map);state->setOutLinks(map);state->setNoneP2PLinks(map);state->setNotAcceptingConnP2PLinks(map);state->setAllLinks(map);state->setMyMinimaAddress(i%3?peer:nullptr);state->setIpReqSecret(std::make_shared<MiniData>(std::vector<uint8_t>{uint8_t(i%2)}));state->setAcceptingInLinks(i%2);state->setDoingDiscoveryConnection(i%2);state->setStartupComplete(i%2);state->setNoConnect(i%2);state->setHostSet(i%2);state->setMaxNumP2PConnections(i%2);state->setMaxNumNoneP2PConnections(i%2);state->setLoopDelay(i%2);state->getKnownPeers()->add(peer);state->getKnownPeers()->remove(i%2?b:a);}}catch(...){++errors;}});
    for(int r=0;r<readers;++r)threads.emplace_back([&]{ready.count_down();start.wait();try{for(int i=0;i<iterations;++i){auto data=state->toJson();require(!data.get("address").asString().empty(),"Invalid address snapshot");require(state->getMaxNumP2PConnections()<=1&&state->getMaxNumNoneP2PConnections()<=1&&state->getLoopDelay()<=1,"Scalar value torn");auto peer=state->getMyMinimaAddress();if(peer)require(peer->getPort()==9001,"Address reference damaged");auto secret=state->getIpReqSecret();if(secret)require(secret->size()<=1,"Secret reference damaged");(void)state->isAcceptingInLinks();(void)state->isDoingDiscoveryConnection();(void)state->isStartupComplete();(void)state->isNoConnect();(void)state->isHostSet();require(state->getKnownPeersCopy()->size()<=2,"Known-peer snapshot damaged");++reads;}}catch(...){++errors;}});
    ready.wait();start.count_down();for(auto& thread:threads)thread.join();require(errors==0&&reads==readers*iterations,"Concurrent state access failed");

    // Freeze global runtime configuration before starting any workers. Exercise
    // actual manager JSON/address updates against status readers.
    GeneralParams::TEST_PARAMS=false;GeneralParams::NOCONNECT=true;GeneralParams::MINIMA_PORT=9001;
    P2PFunctions::logger=[](const auto&){};P2PParams::LOG_LEVEL=P2PLogLevel::DEBUG;
    auto nio=NIOManagerFixture::create();NIOManager::runtimeLookup=[nio]{return nio;};NIOClient::runtimeLookup=[nio]{return nio->clientRuntime();};
    auto client=std::make_shared<NIOClient>(false,"127.0.0.1",9001,nullptr,std::make_shared<NativeNIOSelectionKey>([]{}));NIOClientFixture::uid(*client,"c0");nio->add(client,true);
    auto db=std::make_shared<P2PDB>();std::weak_ptr<P2PManager> target;std::mutex targetMutex;std::promise<void> drained;std::atomic<bool> completed=false;
    P2PManagerServices services;services.getP2PDB=[db]{return db;};services.saveP2PDB=[]{};services.getTip=[]{return std::shared_ptr<TxPoWTreeNode>{};};
    services.releaseLocks=[&]{std::shared_ptr<P2PManager> manager;{std::lock_guard lock(targetMutex);manager=target.lock();}if(manager){auto last=manager->getLastMessage();if(last&&last->isMessageType("BARRIER")&&!completed.exchange(true))drained.set_value();}};
    TimerProcessor::createTimerProcessor();auto timer=TimerProcessor::getTimerProcessor();auto manager=P2PManager::create(services);
    {std::lock_guard lock(targetMutex);target=manager;}
    std::latch readersReady(readers),run(1);threads.clear();reads=0;
    for(int r=0;r<readers;++r)threads.emplace_back([&]{readersReady.count_down();run.wait();try{for(int i=0;i<iterations;++i){auto status=manager->getStatus(i%2);require(status.containsKey(i%2?"p2p_state":"address"),"Status shape damaged");require(!manager->getP2PAddress().empty(),"Empty manager address");(void)manager->getClients();(void)manager->getPeersCopy();manager->LAST_SAVED_PEERS_AMOUNT=i;manager->INITIAL_PEERS_LIST_NUM=i;manager->P2P_HEALTH_CHECK_TIMER=600000+i;++reads;}}catch(...){++errors;}});
    readersReady.wait();run.count_down();
    for(int i=0;i<iterations;++i){auto body=std::make_shared<JSONObject>();body->put("res_ip",i%2?"127.0.0.1":"127.0.0.2");body->put("secret",MiniData().to0xString());auto wrapped=std::make_shared<JSONObject>(SwapLinksFunctions::wrapP2PMsg(body));auto message=std::make_shared<Message>(P2PFunctions::P2P_MESSAGE);message->addString("uid","c0").addObject("message",MessageObject(wrapped));manager->PostMessage(message);}
    manager->PostMessage("BARRIER");auto drainedReady=drained.get_future().wait_for(std::chrono::seconds(15))==std::future_status::ready;for(auto& thread:threads)thread.join();manager->stopMessageProcessor();manager->joinMessageProcessor();timer->stop();timer->join();
    auto checker=manager->getPeersChecker();checker->stopMessageProcessor();checker->joinMessageProcessor();
    require(drainedReady,"Manager did not drain updates");require(errors==0&&reads==readers*iterations,"Concurrent manager status access failed");require(manager->getP2PAddress()=="127.0.0.1:9001","Final manager address update lost");
    {std::lock_guard lock(targetMutex);target.reset();}manager.reset();NIOManager::runtimeLookup={};NIOClient::runtimeLookup={};P2PFunctions::logger={};
    std::cout<<"2000 reference/scalar replacements, 2000 actual manager updates and 16000 concurrent snapshots passed\n";
}
