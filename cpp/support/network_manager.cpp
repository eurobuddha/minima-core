#include "PeerRuntimeFixture.hpp"
#include "org/minima/system/network/NetworkManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima {
class NetworkManagerFixture : public NetworkManager {
    NetworkManagerFixture(NetworkManagerServices services):NetworkManager(std::move(services)){}
public:
    static auto create(NetworkManagerServices services,const std::shared_ptr<NIOManager>& nio){auto manager=std::shared_ptr<NetworkManagerFixture>(new NetworkManagerFixture(std::move(services)));manager->mNIOManager=nio;return manager;}
};
}
static int64_t number(const JSONObject& value,const char* key,int64_t fallback=0){return value.containsKey(key)?std::get<int64_t>(value.get(key).value):fallback;}
static bool flag(const JSONObject& value,const char* key){return value.containsKey(key)&&value.get(key).asBool();}
int main(){std::string line;while(std::getline(std::cin,line))try{
    auto request=object(JSONValue::parseWithException(line));GeneralParams::MINIMA_HOST=request->get("host").asString();GeneralParams::IS_HOST_SET=flag(*request,"hostset");GeneralParams::MINIMA_PORT=int32_t(number(*request,"port",9001));GeneralParams::RPC_PORT=int32_t(number(*request,"rpcport",9005));GeneralParams::RPC_ENABLED=flag(*request,"rpc");GeneralParams::P2P_ENABLED=false;GeneralParams::P2P2_ENABLED=flag(*request,"p2p2");
    int calls=0;NetworkManagerServices services;services.currentTimeMillis=[]{return int64_t(1700000000000);};services.getUserDB=[&]{++calls;if(flag(*request,"failDB"))throw std::runtime_error("DB failure");return flag(*request,"nullDB")?nullptr:std::make_shared<UserDB>();};
    auto nio=NIOManagerFixture::create();std::weak_ptr<NIOManager> weak=nio;NIOClient::runtimeLookup=[weak]{return weak.lock()->clientRuntime();};
    for(int kind=0;kind<2;++kind)for(int i=0;i<number(*request,kind?"connected":"connecting");++i){auto client=std::make_shared<NIOClient>("127.0.0.1",i);NIOClientFixture::uid(*client,std::to_string(kind)+":"+std::to_string(i));nio->add(client,kind);}
    auto traffic=nio->getTrafficListener();traffic->resetAt(1700000000000-number(*request,"age"));traffic->addToTotalRead(number(*request,"read"));traffic->addToTotalWrite(number(*request,"write"));traffic->addReadBytes("RPC",number(*request,"read"));traffic->addWriteBytes("NIO",number(*request,"write"));
    auto manager=NetworkManagerFixture::create(services,flag(*request,"nullNIO")?nullptr:nio);JSONObject result;JSONValue status;
    try{if(request->get("op").asString()=="host"){manager->calculateHostIP();status=GeneralParams::MINIMA_HOST;}else status=manager->getStatus(flag(*request,"all"));}catch(const std::exception&){status="ERROR";}
    result.put("result",status);result.put("dbCalls",calls);std::cout<<result.toJSONString()<<std::endl;manager.reset();nio.reset();NIOClient::runtimeLookup={};
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
