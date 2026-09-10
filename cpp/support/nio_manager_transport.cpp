#include "PeerHarness.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "support/NIOProtocol.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
namespace minima {
class NIOManagerFixture {
public:
    static std::shared_ptr<NIOManager> create(){
        NIOManagerServices services;services.releaseLocks=[]{};
        auto manager=std::shared_ptr<NIOManager>(new NIOManager(services));
        manager->mNIOServer=std::make_shared<NIOServer>(0,std::make_shared<MessageStack>());
        return manager;
    }
};
}
static JSONValue run(const JSONObject& request){
    auto manager=NIOManagerFixture::create();
    NIOManager::runtimeLookup=[manager]{return manager;};
    struct Reset{~Reset(){NIOManager::runtimeLookup={};}} reset;
    JSONObject out;
    try {
        auto mode=request.get("mode").asString();
        if(mode=="ping"){
            auto greeting=NIOManager::sendPingMessage(string(request.get("host")),int32_t(std::get<int64_t>(request.get("port").value)),true);
            out.put("result",greeting?JSONValue(MiniData(greeting->serialise()).to0xString()):JSONValue{});
        }else{
            MiniByte type(int32_t(std::get<int64_t>(request.get("type").value)));
            out.put("name",nioMessageType(type));
            auto kind=request.get("kind").asString();
            std::function<wire::Bytes()> serialize;
            if(kind=="data")serialize=[&]{return MiniData::fromHex(request.get("value").asString()).serialise();};
            else if(kind=="string")serialize=[&]{return MiniString(request.get("value").asString()).serialise();};
            else if(kind=="greeting")serialize=[]{Greeting greeting;greeting.setTopBlock(MiniNumber(123));greeting.getExtraData().put("welcome","fixture");greeting.getChain().push_back(MiniData::fromHex("0xAB"));return greeting.serialise();};
            else if(kind=="failure")serialize=[&]()->wire::Bytes{out.put("serialized",true);throw wire::IOException("Fixture failure");};
            else if(kind=="oom")serialize=[&]()->wire::Bytes{out.put("serialized",true);throw std::bad_alloc();};
            else if(kind=="null")serialize=[]{throw std::runtime_error("Null object");return wire::Bytes{};};
            else throw std::invalid_argument("Unknown object kind");
            if(mode=="create")out.put("result",NIOManager::createNIOMessage(type,serialize).to0xString());
            else{
                if(kind=="null")NIOManager::sendNetworkMessage(string(request.get("uid")),type,std::shared_ptr<MiniData>{});
                else{
                    struct Object{std::function<wire::Bytes()> callback;wire::Bytes serialise()const{return callback();}} value{serialize};
                    NIOManager::sendNetworkMessage(string(request.get("uid")),type,value);
                }
                out.put("result","OK");
            }
        }
    }catch(const wire::IOException& error){out.put("result","IO:"+std::string(error.what()));}
    catch(const std::exception&){out.put("result","ERROR");}
    out.put("traffic",manager->getTrafficListener()->getBreakdown());
    out.put("read",manager->getTrafficListener()->getTotalRead());
    out.put("write",manager->getTrafficListener()->getTotalWrite());
    return out;
}
int main(){GeneralParams::NETWORKING_LOGS=false;std::string line;while(std::getline(std::cin,line))try{std::cout<<run(*object(JSONValue::parseWithException(line))).toJSONString()<<std::endl;}catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
