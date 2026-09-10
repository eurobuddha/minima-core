#include "org/minima/system/network/minima/NIOServer.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "support/NIOConnections.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <condition_variable>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {
class NIOManagerFixture {
public:
    static void add(NIOServer& server,const std::shared_ptr<NIOClient>& client){server.mClients.put(*client->getUID(),client);}
    static size_t registers(NIOServer& server){return server.mRegisterChannels.size();}
    static size_t disconnects(NIOServer& server){return server.mDisconnectChannels.size();}
};
struct NIOClientFixture {static size_t queued(NIOClient& c){return c.mMessages->size();}};
}
struct Context {
    std::mutex lock;std::condition_variable changed;JSONArray events;std::string stage,kind,logKind;int failLog=0,prints=0;bool ready=false,connected=false;
    NIOServer* server=nullptr;std::shared_ptr<NIOClient> client;
    void record(const std::string& text){std::lock_guard guard(lock);events.add(text);}
    void signal(bool& flag){{std::lock_guard guard(lock);flag=true;}changed.notify_all();}
    void wait(bool& flag){std::unique_lock guard(lock);if(!changed.wait_for(guard,std::chrono::seconds(5),[&]{return flag;}))throw std::runtime_error("Fixture timeout");}
    [[noreturn]] static void fail(const std::string& kind,const std::string& text){if(kind=="error")throw JavaError("java.lang.LinkageError",text);if(kind=="oom")throw JavaOutOfMemoryError(text);if(kind=="io")throw wire::IOException(text);throw JavaTypedException("java.lang.IllegalStateException",text);}
    void post(const std::shared_ptr<Message>& message){
        auto type=*message->getMessageType();record("post:"+type+(message->exists("reconnect")?(message->getBoolean("reconnect")?":true":":false"):""));
        if(type=="NIO_NEWCONNECT"){client=message->getObject("client").as<std::shared_ptr<NIOClient>>();signal(connected);}
        bool target=(stage=="startup"&&type=="NIO_SERVERSTARTED")||((stage=="accept"||stage=="register")&&type=="NIO_NEWCONNECT")||(stage=="read"&&type=="NIO_NEWMSG")||(stage=="disconnect"&&type=="NIO_DISCONNECTED");
        if(target){server->shutdown();fail(kind,"fixture");}if(type=="NIO_SERVERSTARTED")signal(ready);
    }
};
struct Owner:MinimaLogger::MainReceiver {
    std::shared_ptr<Context> state;explicit Owner(std::shared_ptr<Context> s):state(std::move(s)){}
    void PostNotifyEvent(const std::string&,std::shared_ptr<JSONObject> data)override{auto text=data->get("message").asString();text=text.substr(text.find(" : ")+3);if(!text.starts_with("     "))state->record("notify:"+text);}
};
struct Handler:NativeUncaughtExceptionHandler {
    std::shared_ptr<Context> state;explicit Handler(std::shared_ptr<Context> s):state(std::move(s)){}
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& e)override{state->record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(*e->toString()));}
};
struct Channel:NIOSocket {bool closed=false;int32_t read(std::span<uint8_t>)override{return 0;}int32_t write(std::span<const uint8_t>)override{return 0;}void close()override{closed=true;}};
struct Key:NIOSelectionKey {
    std::shared_ptr<Context> state;explicit Key(std::shared_ptr<Context> s):state(std::move(s)){}bool isValid()const override{return true;}void wakeup()override{}
    void interestOps(int32_t)override{state->record("interest");Context::fail(state->kind,"fixture");}void cancel()override{state->record("cancel");Context::fail(state->kind,"fixture");}
};
int main(int argc,char** argv)try{
    if(argc!=6)return 2;auto state=std::make_shared<Context>();state->stage=argv[1];state->kind=argv[2];state->failLog=std::stoi(argv[3]);state->logKind=argv[4];NIOServer::mTraceON=std::string(argv[5])=="true";
    auto owner=std::make_shared<Owner>(state);MinimaLogger::Runtime logger;logger.getMain=[owner]{return owner;};logger.println=[state](const auto& full){auto text=unicode::encodeUTF8(full);text=text.substr(text.find(" : ")+3);if(text.starts_with("     "))return;state->record("log:"+text);if(++state->prints==state->failLog){state->server->shutdown();Context::fail(state->logKind,"logger");}};MinimaLogger::setRuntime(logger);
    auto runtime=std::make_shared<NIORuntime>();runtime->traffic=std::make_shared<NIOTraffic>();runtime->postMessage=[state](auto message){state->post(message);};NIOClient::runtimeLookup=[runtime]{return runtime;};NIOServer server(0,[state](auto message){state->post(message);});state->server=&server;
    if(state->stage=="send"||state->stage=="all"||state->stage=="cancel"){
        auto channel=std::make_shared<Channel>();auto key=std::make_shared<Key>(state);auto client=std::make_shared<NIOClient>(true,"127.0.0.1",1,channel,key);client->setValidGreeting(true);NIOManagerFixture::add(server,client);
        try{if(state->stage=="cancel")client->disconnect();else if(state->stage=="all")server.sendMessageAll(std::make_shared<MiniData>(std::vector<uint8_t>{1}));else server.sendMessage(client->getUID(),std::make_shared<MiniData>(std::vector<uint8_t>{1}));state->record("returned");}catch(const std::exception& e){state->record("escaped:"+nativeExceptionText(e));}
        state->record("queued:"+std::to_string(NIOClientFixture::queued(*client)));state->record(std::string("closed:")+(channel->closed?"true":"false"));
    }else{
        setDefaultUncaughtExceptionHandler(std::make_shared<Handler>(state));std::thread worker([&]{runNativeThread(NativeThread{u"nio-fixture"},[&]{server.run();});});
        struct Stop{NIOServer& server;std::thread& worker;~Stop(){server.shutdown();if(worker.joinable())worker.join();}}stop{server,worker};
        std::shared_ptr<PosixNIOSocket> peer,outgoing;
        if(state->stage!="startup"){
            state->wait(state->ready);
            if(state->stage=="register"){
                int fd=::socket(AF_INET,SOCK_STREAM,0);if(fd<0)throw std::runtime_error("socket");struct Close{int fd;~Close(){::close(fd);}}close{fd};sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
                if(::bind(fd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))||::listen(fd,1))throw std::runtime_error("bind/listen");socklen_t n=sizeof(addr);if(::getsockname(fd,reinterpret_cast<sockaddr*>(&addr),&n))throw std::runtime_error("getsockname");outgoing=nioConnect(InetSocketAddress("127.0.0.1",ntohs(addr.sin_port)),1000);int accepted=::accept(fd,nullptr,nullptr);if(accepted<0)throw std::runtime_error("accept");peer=std::make_shared<PosixNIOSocket>(accepted);server.regsiterNewSocket(outgoing);
            }else peer=nioConnect(InetSocketAddress("127.0.0.1",server.getBoundPort()),1000);
            if(state->stage!="accept"&&state->stage!="register"){state->wait(state->connected);if(state->stage=="read")nioWriteAll(peer,std::vector<uint8_t>{0,0,0,1,42});else server.disconnect(state->client->getUID());}
        }
        worker.join();state->record(std::string("running:")+(server.isRunning()?"true":"false"));state->record("registers:"+std::to_string(NIOManagerFixture::registers(server)));state->record("disconnects:"+std::to_string(NIOManagerFixture::disconnects(server)));setDefaultUncaughtExceptionHandler(nullptr);
    }
    state->record("clients:"+std::to_string(server.getNetClientSize()));JSONObject result;result.put("events",state->events);std::cout<<result.toJSONString()<<'\n';state->client.reset();NIOClient::runtimeLookup={};MinimaLogger::setRuntime({});
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
