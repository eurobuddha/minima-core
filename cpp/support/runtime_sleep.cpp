#include "PeerRuntimeFixture.hpp"
#include "SqlRecordingFixture.hpp"
#include "SystemCheckFixture.hpp"
#include "org/minima/system/commands/network/rpc.hpp"
#include <iostream>
using namespace minima;
namespace minima {
// Adapt the existing MainNetworkFixture's unstarted owner; all three completion
// receivers share one controlled NIO fixture for this Main wait-loop test.
class NetworkManagerFixture:public NetworkManager {
    NetworkManagerFixture():NetworkManager(NetworkManagerServices{}){}
public:
    static auto create(const std::shared_ptr<NIOManager>& nio){auto net=std::shared_ptr<NetworkManagerFixture>(new NetworkManagerFixture);net->mNIOManager=nio;net->mP2PManager=nio;net->mP2P2Manager=nio;return net;}
};
}
class ManualMain:public Main {
    static MainServices services(){MainServices s;s.sql.connect=[](const auto& url,const auto& user,const auto& password)->std::shared_ptr<SqlConnection>{sql_fixture::event("connect",{url,user,password});return std::make_shared<sql_fixture::Connection>();};return s;}
public:ManualMain():Main(services()){}
};
int run(int argc,char** argv,const NativeThread& caller)try{
    if(argc!=2)return 2;std::string mode=argv[1];JSONObject out;out.put("mode",mode);
    GeneralParams::resetDefaults();GeneralParams::P2P_ENABLED=false;GeneralParams::P2P2_ENABLED=false;GeneralParams::RPC_ENABLED=false;
    GeneralParams::NOCONNECT=true;GeneralParams::CONNECT_LIST="";GeneralParams::IS_HOST_SET=true;GeneralParams::MINIMA_HOST="127.0.0.1";GeneralParams::MINIMA_PORT=0;
    MinimaLogger::Runtime logger;logger.println=[&](const auto& text){std::cerr<<unicode::encodeUTF8(text)<<'\n';if(mode=="restart_wait"&&unicode::encodeUTF8(text).find("restart in 5 seconds")!=std::string::npos)interruptCurrentNativeThread();};MinimaLogger::setRuntime(logger);
    JSONObject sqlConfig;sql_fixture::config=&sqlConfig;
    auto main=std::make_shared<ManualMain>();MainFixture::install(main);MainFixture::initialize(main);
    auto nio=NIOManagerFixture::create();auto network=NetworkManagerFixture::create(nio);MainFixture::network(*main,network);JSONObject config;SystemCheckFixture::setup(main,config);
    // Keep the logger boundary but avoid callback notifications from unrelated
    // network threads entering the unstarted diagnostic NotifyManager.
    MinimaLogger::setRuntime(logger);
    std::string error="none";std::thread completed;
    if(mode=="restart_pause"){nio->stopMessageProcessor();nio->run();}
    interruptCurrentNativeThread();
    // Release the old network only after the caller has consumed the interrupt.
    // This forces the wait-loop branch without depending on a scheduling delay.
    if(mode=="shutdown_wait"||mode=="restart_wait")completed=std::thread([nio,&caller]{while(caller.isInterrupted())std::this_thread::sleep_for(std::chrono::milliseconds(1));nio->stopMessageProcessor();nio->run();});
    try{
        if(mode=="shutdown_wait")MainFixture::shutdownGeneral(*main);
        else if(mode=="restart_wait"||mode=="restart_pause")main->restartNIO();
        else if(mode=="netrestart")MainFixture::dispatch(*main,std::make_shared<Message>(Main::MAIN_NETRESTART));
        else if(mode=="rpc"){
            commands::rpc command;command.getParams()->put("enable","true");command.getParams()->put("password","synthetic");command.runCommand();
        }else throw std::runtime_error("unknown mode");
    }catch(const JavaTypedException& e){error=e.className();}
    if(completed.joinable())completed.join();
    out.put("error",error);out.put("pending",nativeThreadInterrupted());out.put("replaced",main->getNetworkManager()!=network);
    out.put("enabled",GeneralParams::RPC_ENABLED.load());out.put("authenticate",GeneralParams::RPC_AUTHENTICATE.load());
    out.put("locked",MinimaDB::getDB()->getRWLockInfo().find("Read locks = 0]")==std::string::npos);
    auto current=main->getNetworkManager();if(current!=network){current->shutdownNetwork();current->join();}
    Main::ClearMainInstance();main.reset();MinimaLogger::setRuntime({});std::cout<<out.toJSONString()<<'\n';return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
int main(int argc,char** argv){int result=1;NativeThread caller{u"runtime-sleep-caller"};runNativeThread(caller,[&]{result=run(argc,argv,caller);});return result;}
