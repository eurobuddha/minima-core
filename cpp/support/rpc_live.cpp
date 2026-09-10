#include "SqlRecordingFixture.hpp"
#include "MainFixture.hpp"
#include "MainNetworkFixture.hpp"
#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include <iostream>
using namespace minima;
using namespace minima::json;
namespace {
class ManualMain:public Main {
    static MainServices services(){MainServices s;s.sql.connect=[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{return std::make_shared<sql_fixture::Connection>();};return s;}
public:ManualMain():Main(services()){}
};
JSONObject state(const std::shared_ptr<NetworkManager>& network){
    JSONObject out;out.put("bound",network->getBoundRPCPort());out.put("enabled",GeneralParams::RPC_ENABLED.load());out.put("authenticate",GeneralParams::RPC_AUTHENTICATE.load());
    auto password=GeneralParams::RPC_PASSWORD.load();out.put("password",password?JSONValue(*password):JSONValue());out.put("users",*MinimaDB::getDB()->getUserDB()->getRPCUsers());return out;
}
}
int main()try{
    std::cout<<std::unitbuf;JSONObject empty;sql_fixture::config=&empty;
    auto logger=MinimaLogger::getRuntime();logger.println=[](const auto& text){std::clog<<unicode::encodeUTF8(text)<<'\n';};MinimaLogger::setRuntime(std::move(logger));
    GeneralParams::resetDefaults();GeneralParams::RPC_PORT=0;GeneralParams::P2P_ENABLED=false;GeneralParams::P2P2_ENABLED=false;
    auto main=std::make_shared<ManualMain>();MainFixture::install(main);MainFixture::initialize(main);
    auto fixture=support::MainNetworkFixture::create(empty,[&](auto network){MainFixture::network(*main,std::move(network));});auto network=fixture->network();
    try{std::string line;while(std::getline(std::cin,line)){
        auto input=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);auto op=input->getString("op");JSONObject out;
        auto before=std::chrono::steady_clock::now();
        if(op=="command")out.put("result",CommandRunner::getRunner().runMultiCommand(input->getString("text")));
        else if(op=="configure")GeneralParams::RPC_CRLF=input->getBoolean("crlf");
        else if(op=="replaceDB")MainFixture::database(main);
        else if(op=="wait"){
            auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);
            while(!network->getBoundRPCPort()){if(std::chrono::steady_clock::now()>end)throw std::runtime_error("RPC listener did not bind");std::this_thread::sleep_for(std::chrono::milliseconds(2));}
        }else if(op=="client"){
            auto url="http://127.0.0.1:"+std::to_string(network->getBoundRPCPort())+"/"+input->getString("path");
            out.put("result",RPCClient::sendGETBasicAuth(url,input->getString("user"),input->getString("password")));
        }else if(op!="state")throw std::invalid_argument("Unknown operation");
        out.put("elapsed",std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-before).count());out.put("state",state(network));std::cout<<out.toJSONString()<<'\n';
    }}catch(...){network->stopRPC();Main::ClearMainInstance();throw;}
    network->stopRPC();Main::ClearMainInstance();
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
