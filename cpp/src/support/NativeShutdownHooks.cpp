#include "NativeShutdownHooks.hpp"
#include "NativeThread.hpp"
#include "JavaExceptions.hpp"
#include "ByteIO.hpp"
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
namespace minima {
namespace {
volatile sig_atomic_t signalPipe=-1;
void signalHandler(int signal){int saved=errno;unsigned char value=static_cast<unsigned char>(signal);if(signalPipe>=0)(void)::write(int(signalPipe),&value,1);errno=saved;}
struct Hooks {
    struct Hook {std::u16string name;std::function<void()> run;};
    std::mutex lock;
    std::vector<Hook> hooks;
    std::once_flag once;
    bool stopping=false;
    int pipes[2]{-1,-1};
    std::thread dispatcher;
    void run(){std::call_once(once,[&]{
        std::vector<Hook> copy;{std::lock_guard guard(lock);stopping=true;copy=hooks;}
        // jthread joins already-started hooks if creating a later worker fails.
        std::vector<std::jthread> workers;workers.reserve(copy.size());
        for(auto& hook:copy)workers.emplace_back([hook]{runNativeThread(NativeThread{hook.name},hook.run);});
    });}
};
// The process owns this registry and its signal dispatcher until exit. Keeping
// it alive avoids static-destructor ordering races with a terminating signal.
Hooks& registry(){static auto* value=new Hooks();return *value;}
void processExit(){registry().run();}
void install(){
    auto& value=registry();
    const int signals[]{SIGHUP,SIGINT,SIGTERM};struct sigaction previous[3]{};size_t installed=0;
    try{
        if(::pipe(value.pipes)<0)throw wire::IOException(std::strerror(errno));
        for(int descriptor:value.pipes)if(::fcntl(descriptor,F_SETFD,FD_CLOEXEC)<0)throw wire::IOException(std::strerror(errno));
        int flags=::fcntl(value.pipes[1],F_GETFL);if(flags<0||::fcntl(value.pipes[1],F_SETFL,flags|O_NONBLOCK)<0)throw wire::IOException(std::strerror(errno));
        signalPipe=value.pipes[1];
        struct sigaction action{};action.sa_handler=signalHandler;sigemptyset(&action.sa_mask);
        for(int signal:signals){if(::sigaction(signal,&action,&previous[installed])<0)throw wire::IOException(std::strerror(errno));++installed;}
        if(std::atexit(processExit)!=0)throw wire::IOException("Cannot register shutdown hooks");
        // The process-lifetime registry owns the dispatcher. No detached-thread
        // handoff can throw after a worker has successfully started.
        value.dispatcher=std::thread([&value]{unsigned char signal;ssize_t count;do{count=::read(value.pipes[0],&signal,1);}while(count<0&&errno==EINTR);if(count==1){value.run();std::exit(128+signal);}});
    }catch(...){
        while(installed){--installed;::sigaction(signals[installed],&previous[installed],nullptr);}
        signalPipe=-1;
        for(int& descriptor:value.pipes){if(descriptor>=0)::close(descriptor);descriptor=-1;}
        throw;
    }
}
}
void addNativeShutdownHook(std::function<void()> hook){
    if(!hook)throw JavaNullPointerException("null shutdown hook");
    static std::once_flag installed;std::call_once(installed,install);
    auto& value=registry();std::lock_guard guard(value.lock);if(value.stopping)throw JavaTypedException("java.lang.IllegalStateException","Shutdown in progress");
    value.hooks.push_back({nextNativeThreadName(),std::move(hook)});
}
}
