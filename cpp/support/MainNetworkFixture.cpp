#include "MainNetworkFixture.hpp"
#include "P2PManagerFixture.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima {
class NetworkManagerFixture:public NetworkManager {
    NetworkManagerFixture():NetworkManager([]{NetworkManagerServices services;services.getUserDB=[]{return sqlRequired(MinimaDB::getDB()).getUserDB();};return services;}()){}
public:
    static void setNIO(const std::shared_ptr<NetworkManager>& manager,std::shared_ptr<NIOManager> nio){manager->mNIOManager=std::move(nio);}
    static auto create(const std::shared_ptr<NIOManager>& nio,const std::shared_ptr<MessageProcessor>& peer){auto result=std::shared_ptr<NetworkManagerFixture>(new NetworkManagerFixture);result->mNIOManager=nio;result->mP2PManager=peer;return result;}
};
}
namespace minima::support {
namespace {
struct Fixture:MainNetworkFixture {
    std::shared_ptr<NIOManagerFixture> nio=NIOManagerFixture::create();
    std::shared_ptr<p2p::P2PManagerFixture> peer;
    std::shared_ptr<NetworkManager> net;
    std::vector<std::shared_ptr<NIOClient>> clients;
    bool statusFixture=false;
    explicit Fixture(const json::JSONObject& config,const std::function<void(std::shared_ptr<NetworkManager>)>& publish){
        statusFixture=config.containsKey("statusFixture")&&config.get("statusFixture").asBool();
        if(statusFixture){
            MessageProcessor::setTrace(false,"");GeneralParams::MINIMA_HOST=config.getString("host","fixture-host");GeneralParams::IS_HOST_SET=config.containsKey("hostset")&&config.get("hostset").asBool();
            GeneralParams::MINIMA_PORT=9001;GeneralParams::RPC_PORT=9005;GeneralParams::RPC_ENABLED=false;
            auto traffic=nio->getTrafficListener();traffic->resetAt(0);traffic->addToTotalRead(12345);traffic->addToTotalWrite(23456);traffic->addReadBytes("fixture-read",1024);traffic->addWriteBytes("fixture-write",2048);
        }
        p2p::P2PManagerServices services;services.releaseLocks=[]{};peer=p2p::P2PManagerFixture::create(services);
        auto kind=config.getString("peer","real");std::shared_ptr<MessageProcessor> selected=peer;if(kind=="null")selected.reset();else if(kind=="wrong")selected=nio;
        net=NetworkManagerFixture::create(nio,selected);publish(net);
        if(config.containsKey("state")){auto values=::object(config.get("state"));auto state=peer->peerState();if(values->containsKey("accept"))state->setAcceptingInLinks(values->get("accept").asBool());if(values->containsKey("startup"))state->setStartupComplete(values->get("startup").asBool());if(values->containsKey("discovery"))state->setDoingDiscoveryConnection(values->get("discovery").asBool());}
        if(config.containsKey("clients"))for(const auto& raw: ::array(config.get("clients"))->elements()){
            auto row=::object(raw);auto key=std::make_shared<NativeNIOSelectionKey>([]{});
            auto client=std::make_shared<NIOClient>(false,"127.0.0.1",9001,nullptr,key);NIOClientFixture::uid(*client,row->getString("uid"));client->setValidGreeting(row->get("valid").asBool());
            clients.push_back(client);nio->add(client,row->get("connected").asBool());
        }
        if(config.containsKey("nullNIO")&&config.get("nullNIO").asBool())NetworkManagerFixture::setNIO(net,nullptr);
    }
    std::shared_ptr<NetworkManager> network() override{return net;}
    json::JSONObject snapshot() override{json::JSONObject out;out.put("queued",nio->drain());json::JSONArray rows;for(auto& client:clients){json::JSONObject row;row.put("uid",client->getUID().value());row.put("packets",NIOClientFixture::packets(*client));rows.add(row);}out.put("clients",rows);out.put("traffic",nio->getTrafficListener()->getBreakdown());
        if(statusFixture){auto traffic=nio->getTrafficListener();out.put("trace",nio->isTrace());out.put("filter",nio->getTraceFilter());out.put("host",GeneralParams::MINIMA_HOST);out.put("hostset",GeneralParams::IS_HOST_SET);out.put("totalRead",traffic->getTotalRead());out.put("totalWrite",traffic->getTotalWrite());out.put("start",traffic->getStartTime());}return out;}
};
}
std::shared_ptr<MainNetworkFixture> MainNetworkFixture::create(const json::JSONObject& config,const std::function<void(std::shared_ptr<NetworkManager>)>& publish){return std::make_shared<Fixture>(config,publish);}
}
