#include "PeerHarness.hpp"
#include "org/minima/system/network/rpc/HTTPServer.hpp"
#include "org/minima/system/network/p2p/P2PFunctions.hpp"
#include "support/NIOConnections.hpp"
#include "org/minima/system/network/rpc/CMDHandler.hpp"
#include "org/minima/system/network/rpc/Authorizer.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <condition_variable>
#include "org/minima/utils/MinimaLogger.hpp"
#include "support/JavaUnicode.hpp"
struct Context {
    std::mutex lock;std::condition_variable changed;JSONArray logs;
    bool ready=false,finished=false,rpc=false;int factories=0,active=0,completed=0,maximum=0;
};
class Listener : public HTTPServer {
    std::shared_ptr<Context> context;
    Listener(int port,std::shared_ptr<Context> state):HTTPServer(port),context(std::move(state)){}
public:
    static auto create(int port,const std::shared_ptr<Context>& context,bool autoStart){auto value=std::shared_ptr<Listener>(new Listener(port,context));if(autoStart)value->start();return value;}
    void run()override{HTTPServer::run();std::lock_guard lock(context->lock);context->finished=true;context->changed.notify_all();}
    std::function<void()> getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket)override{
        {std::lock_guard lock(context->lock);++context->factories;}
        return [state=context,socket]{
            {std::lock_guard lock(state->lock);++state->active;state->maximum=std::max(state->maximum,state->active);state->changed.notify_all();}
            try{if(state->rpc)CMDHandler(socket).run();else{auto bytes=nioReadAll(socket,5,5000);std::reverse(bytes.begin(),bytes.end());nioWriteAll(socket,bytes);}}catch(const std::exception&){}
            socket->close();{std::lock_guard lock(state->lock);--state->active;++state->completed;state->changed.notify_all();}
        };
    }
};
int main(){
    auto context=std::make_shared<Context>();std::shared_ptr<Listener> server;
    auto capture=[context](const std::string& message){std::lock_guard lock(context->lock);context->logs.add(message);context->ready=true;context->changed.notify_all();};
    // P2P/CMDHandler still expose their legacy log receiver; HTTPServer now
    // uses the actual logger. Keep protocol replies separate from both paths.
    p2p::P2PFunctions::logger=capture;
    MinimaLogger::Runtime runtime;runtime.println=[capture](const auto& full){auto text=unicode::encodeUTF8(full);capture(text.substr(text.find(" : ")+3));};MinimaLogger::setRuntime(runtime);
    std::string line;while(std::getline(std::cin,line)){try{
        auto request=object(JSONValue::parseWithException(line));auto kind=request->get("op").asString();
        if(kind=="create"){
            context->rpc=request->containsKey("rpc")&&request->get("rpc").asBool();
            if(context->rpc){auto db=std::make_shared<UserDB>();if(request->containsKey("users"))db->getAllData()->put("rpcusers_allusers",request->get("users"));Authorizer::userDB=[db]{return db;};GeneralParams::RPC_AUTHENTICATE=request->get("authenticate").asBool();GeneralParams::RPC_CRLF=request->get("crlf").asBool();GeneralParams::RPC_PASSWORD="test-password";GeneralParams::RPC_AUTHSTYLE=request->get("style").asString();}
            bool autoStart=request->get("auto").asBool();server=Listener::create(int(std::get<int64_t>(request->get("port").value)),context,autoStart);if(autoStart){std::unique_lock lock(context->lock);if(!context->changed.wait_for(lock,std::chrono::seconds(5),[&]{return (context->ready&&server->getBoundPort()!=0)||context->finished;}))throw std::runtime_error("Listener startup timed out");}}
        else if(kind=="start"){server->start();std::unique_lock lock(context->lock);if(!context->changed.wait_for(lock,std::chrono::seconds(5),[&]{return (context->ready&&server->getBoundPort()!=0)||context->finished;}))throw std::runtime_error("Listener startup timed out");}
        else if(kind=="status"){auto target=std::get<int64_t>(request->get("completed").value);auto active=request->containsKey("active")?std::get<int64_t>(request->get("active").value):int64_t(0);std::unique_lock lock(context->lock);if(!context->changed.wait_for(lock,std::chrono::seconds(5),[&]{return context->completed>=target&&context->active>=active;}))throw std::runtime_error("Handlers did not finish");}
        else if(kind=="shutdown"){server->shutdown();server->join();}
        else throw std::runtime_error("Unknown listener operation");
        JSONObject row;row.put("port",server->getPort());row.put("bound",server->getBoundPort());{std::lock_guard lock(context->lock);row.put("logs",context->logs);row.put("factories",context->factories);row.put("completed",context->completed);row.put("concurrent",context->maximum>=2);row.put("finished",context->finished);}std::cout<<row.toJSONString()<<std::endl;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"\"ERROR\""<<std::endl;}}
    if(server){server->shutdown();server->join();}p2p::P2PFunctions::logger={};MinimaLogger::setRuntime({});
}
