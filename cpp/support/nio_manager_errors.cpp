#include "org/minima/system/network/minima/NIOManager.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <condition_variable>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima {
struct NIOClientFixture {static void uid(NIOClient& client){client.mUID="fixture";}};
class NIOManagerFixture:public NIOManager {
    NIOManagerFixture():NIOManager([]{NIOManagerServices s;s.releaseLocks=[]{};return s;}()){}
public:
    static auto createBare(){return std::shared_ptr<NIOManagerFixture>(new NIOManagerFixture);}
    void prepare(){mNIOServer=std::make_shared<NIOServer>(0,[](auto){});mNIOServer->shutdown();mNIOServer->run();}
    void launchServer(){startServer();}void join(){joinServer();}
    void attempt(const std::shared_ptr<NIOClient>& client){mConnectingClients.put(*client->getUID(),client);connectAttempt(client);}
    size_t registers(){std::lock_guard lock(mNIOServer->mRegisterLock);return mNIOServer->mRegisterChannels.size();}
    std::shared_ptr<Message> next(){return getNextMessage();}
};
}
struct Context {
    std::mutex lock;std::condition_variable changed;JSONArray events;std::string mode,kind;int prints=0;bool logged=false;
    void record(const std::string& text){std::lock_guard guard(lock);events.add(text);}
    [[noreturn]] static void fail(const std::string& kind,const std::string& text){if(kind=="error")throw JavaError("java.lang.LinkageError",text);if(kind=="oom")throw JavaOutOfMemoryError(text);if(kind=="io")throw wire::IOException(text);throw JavaTypedException("java.lang.IllegalStateException",text);}
};
struct Handler:NativeUncaughtExceptionHandler {
    std::shared_ptr<Context> state;explicit Handler(std::shared_ptr<Context> s):state(std::move(s)){}
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& e)override{state->record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(*e->toString()));}
};
int main(int argc,char** argv)try{
    if(argc!=3)return 2;auto state=std::make_shared<Context>();state->mode=argv[1];state->kind=argv[2];setDefaultUncaughtExceptionHandler(std::make_shared<Handler>(state));
    auto runtime=std::make_shared<NIORuntime>();runtime->traffic=std::make_shared<NIOTraffic>();NIOClient::runtimeLookup=[runtime]{return runtime;};auto manager=NIOManagerFixture::createBare();
    MinimaLogger::Runtime logger;logger.println=[state](const auto& full){auto text=unicode::encodeUTF8(full);text=text.substr(text.find(" : ")+3);if(text.starts_with("     "))return;state->record("log:"+text);{std::lock_guard lock(state->lock);state->logged=true;}state->changed.notify_all();state->prints++;
        if(state->mode=="catch"){if(state->prints==1)Context::fail("runtime","success logger");else Context::fail(state->kind,"catch logger");}else if(state->prints==1)Context::fail(state->kind,"logger");
    };MinimaLogger::setRuntime(logger);
    if(state->mode=="server"){
        GeneralParams::MINIMA_PORT=-1;manager->launchServer();manager->join();state->record(std::string("running:")+(manager->getNIOServer()->isRunning()?"true":"false"));
    }else{
        manager->prepare();int port=-1;struct Close{int fd=-1;~Close(){if(fd>=0)::close(fd);}}listener;
        if(state->mode!="invalid"){
            listener.fd=::socket(AF_INET,SOCK_STREAM,0);if(listener.fd<0)throw std::runtime_error("socket");sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
            if(::bind(listener.fd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))||::listen(listener.fd,1))throw std::runtime_error("bind/listen");socklen_t n=sizeof(addr);if(::getsockname(listener.fd,reinterpret_cast<sockaddr*>(&addr),&n))throw std::runtime_error("getsockname");port=ntohs(addr.sin_port);
        }
        auto client=std::make_shared<NIOClient>("127.0.0.1",port);NIOClientFixture::uid(*client);manager->attempt(client);
        {std::unique_lock lock(state->lock);if(!state->changed.wait_for(lock,std::chrono::seconds(5),[&]{return state->logged;}))throw std::runtime_error("connector log timeout");}
        // The actual detached connector retains its manager until dispatch ends.
        auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);while(manager.use_count()!=1){if(std::chrono::steady_clock::now()>deadline)throw std::runtime_error("connector owner timeout");std::this_thread::yield();}
        while(auto message=manager->next())state->record("post:"+*message->getMessageType());state->record("pending:"+std::to_string(manager->getNumberOfConnnectingClients()));state->record("registers:"+std::to_string(manager->registers()));
    }
    JSONObject result;result.put("events",state->events);std::cout<<result.toJSONString()<<'\n';manager.reset();NIOClient::runtimeLookup={};MinimaLogger::setRuntime({});setDefaultUncaughtExceptionHandler(nullptr);
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
