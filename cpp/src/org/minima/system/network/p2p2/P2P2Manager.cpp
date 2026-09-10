#include "P2P2Manager.hpp"
#include "support/PeerOperations.hpp"
namespace minima::p2p2 {
using p2p::peer_detail::required;
P2P2Manager::P2P2Manager(P2P2ManagerServices services):MessageProcessor("P2P2MANAGER",services.releaseLocks),mServices(std::move(services)){}
void P2P2Manager::initialize(){PostMessage(P2P2_INIT);PostTimerMessage(std::make_shared<TimerMessage>(P2P2_LOOP_TIMER,P2P2_FASTLOOP));PostTimerMessage(std::make_shared<TimerMessage>(P2P2_LOOP_TIMER_SLOW,P2P2_SLOWLOOP));}
std::shared_ptr<P2P2Manager> P2P2Manager::create(P2P2ManagerServices services){auto manager=std::shared_ptr<P2P2Manager>(new P2P2Manager(std::move(services)));manager->initialize();manager->startMessageProcessor();return manager;}
void P2P2Manager::shutdown(){stopMessageProcessor();p2p::P2PFunctions::log("P2P2 shutdown.. ");}
std::optional<std::string> P2P2Manager::getRandomPeerFromList(){auto db=mServices.getP2P2DB();auto peers=required(db).getAllKnownPeers();if(required(peers).empty())return std::string();auto index=JavaRandom48().nextInt(int32_t(peers->size()));return peers->at(size_t(index));}
void P2P2Manager::checkWhichConnect(){/* The pinned Java method has no statements. */}
void P2P2Manager::convertOldP2P(){auto db=mServices.getP2PDB();auto peers=required(db).getPeersList();std::vector<std::string> newpeers;for(const auto& peer:required(peers)){auto host=required(peer).getHostString();auto port=peer->getPort();(void)host;(void)port;}}
void P2P2Manager::processMessage(std::shared_ptr<Message> message){
    const auto type=required(message).getMessageType().value();
    if(type==P2P2_INIT){
        p2p::P2PFunctions::log("P2P2 Inited.. ");
        required(mServices.getP2P2DB()).addPeerToAllKnown("324.45.45.45:9901");
        required(mServices.getP2P2DB()).addPeerToAllKnown("324.45.45.45:9901");
        required(mServices.getP2P2DB()).addPeerToAllKnown("324.45.45.45:9902");
        // Preserve the source read even though its conditional body is empty.
        (void)required(mServices.getP2P2DB()).isFirstStartUp();
    }else if(type==P2P2_FASTLOOP){
        PostTimerMessage(std::make_shared<TimerMessage>(P2P2_LOOP_TIMER,P2P2_FASTLOOP));
        if(!P2P2Functions::isNetAvailable())return;
        auto nio=NIOManager::runtimeLookup();auto connections=required(nio).getNumberOfConnectedClients();
        if(connections<NUMBER_DESIRED_CONNECTIONS){auto peer=getRandomPeerFromList();(void)peer;return;}
    }else if(type==P2P2_SLOWLOOP){/* The pinned Java branch has no statements. */}
}
}
