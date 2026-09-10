#include "org/minima/system/network/rpc/HTTPServer.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaSocketExceptions.hpp"
#include "support/NIOConnections.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <condition_variable>
#include <iostream>
using namespace minima;using namespace minima::json;
namespace {
struct Context {
    std::mutex lock;std::condition_variable changed;JSONArray events;
    std::string stage,kind;int prints=0,failLog=0;bool ready=false,finished=false,handled=false;
    void record(const std::string& text){std::lock_guard guard(lock);events.add(text);}
    void signal(bool& flag){{std::lock_guard guard(lock);flag=true;}changed.notify_all();}
    void wait(bool& flag){std::unique_lock guard(lock);if(!changed.wait_for(guard,std::chrono::seconds(5),[&]{return flag;}))throw std::runtime_error("Fixture timed out");}
    [[noreturn]] void fail(){
        if(kind=="io")throw wire::IOException("fixture");if(kind=="socket")throw wire::SocketException("fixture");if(kind=="bind")throw wire::BindException("fixture");if(kind=="error")throw JavaError("java.lang.LinkageError","fixture");if(kind=="oom")throw JavaOutOfMemoryError("fixture");throw JavaTypedException("java.lang.IllegalStateException","fixture");
    }
};
class Listener:public HTTPServer {
    std::shared_ptr<Context> state;
    Listener(int port,std::shared_ptr<Context> s):HTTPServer(port),state(std::move(s)){}
public:
    static auto create(int port,std::shared_ptr<Context> state){return std::shared_ptr<Listener>(new Listener(port,std::move(state)));}
    void run()override{
        try{HTTPServer::run();}catch(...){state->record("finished");state->signal(state->finished);state->signal(state->ready);throw;}
        state->record("finished");state->signal(state->finished);state->signal(state->ready);
    }
    bool running()const{return mRunning;}
    std::function<void()> getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket)override{
        state->record("factory");socket->close();if(state->stage=="factory"||state->stage=="stopped"){if(state->stage=="stopped")shutdown();state->fail();}
        return [state=state]{state->record("handler");state->fail();};
    }
};
struct Handler:NativeUncaughtExceptionHandler {
    std::shared_ptr<Context> state;
    explicit Handler(std::shared_ptr<Context> value):state(std::move(value)){}
    void uncaughtException(const NativeThread* thread,const std::shared_ptr<MinimaLogger::Throwable>& e)override{state->record("uncaught:"+unicode::encodeUTF8(thread->name)+":"+unicode::encodeUTF8(e->toString().value_or(u"null")));state->signal(state->handled);if(state->kind=="defaultthrows")throw std::runtime_error("default");}
};
}
int main(int argc,char** argv)try{
    if(argc!=5)return 2;auto state=std::make_shared<Context>();state->stage=argv[1];state->kind=argv[2];int port=std::stoi(argv[3]);state->failLog=std::stoi(argv[4]);
    MinimaLogger::Runtime runtime;runtime.println=[state](const auto& full){auto text=unicode::encodeUTF8(full);text=text.substr(text.find(" : ")+3);if(text.starts_with("     "))return;state->record("log:"+text);if(++state->prints==state->failLog)throw JavaIllegalArgumentException("logger");if(text.starts_with("Server started"))state->signal(state->ready);};MinimaLogger::setRuntime(runtime);setDefaultUncaughtExceptionHandler(std::make_shared<Handler>(state));
    auto listener=Listener::create(port,state);
    if(state->stage=="direct"){try{listener->run();}catch(const std::exception& e){state->record("direct:"+nativeExceptionText(e));}}
    else{
        listener->start();state->wait(state->ready);
        if(state->stage!="startup"&&state->stage!="invalid"&&state->stage!="collision"){auto socket=nioConnect(InetSocketAddress("127.0.0.1",listener->getBoundPort()),1000);socket->close();}
        bool caught=(state->stage=="factory"||state->stage=="stopped")&&(state->kind=="io"||state->kind=="socket"||state->kind=="bind")&&state->failLog==0;
        if(caught)state->wait(state->finished);else state->wait(state->handled);
    }
    state->record(std::string("running:")+(listener->running()?"true":"false"));state->record(std::string("bound:")+(listener->getBoundPort()!=0?"true":"false"));listener->shutdown();listener->join();state->wait(state->finished);setDefaultUncaughtExceptionHandler(nullptr);MinimaLogger::setRuntime({});JSONObject result;result.put("events",state->events);std::cout<<result.toJSONString()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
