#include "HTTPServer.hpp"
#include "support/NativeThread.hpp"
#include "support/NativeThrowable.hpp"
#include "support/JavaSocketExceptions.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <cerrno>
#include <cstring>
namespace minima {
namespace {
struct Descriptor{int value;~Descriptor(){if(value>=0)::close(value);}int release(){int result=value;value=-1;return result;}};
wire::SocketException socketError(const char* operation){return wire::SocketException(std::string(operation)+": "+std::strerror(errno));}
}
HTTPServer::~HTTPServer(){shutdown();join();}
void HTTPServer::start(){std::lock_guard lock(mThreadLock);auto self=shared_from_this();auto name=nextNativeThreadName();mThreads.emplace_back([self,name=std::move(name)]()mutable{NativeThread thread{std::move(name)};runNativeThread(thread,[&]{self->run();});});}
void HTTPServer::join(){std::lock_guard lock(mThreadLock);for(auto& thread:mThreads)if(thread.joinable()){if(thread.get_id()==std::this_thread::get_id())thread.detach();else thread.join();}}
void HTTPServer::shutdown(){
    mRunning=false;
    std::lock_guard lock(mSocketLock);
    try{if(mServerSocket)mServerSocket->close();}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    for(const auto& reference:mWakeups)if(auto wake=reference.lock())try{wake->wake();}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
}
void HTTPServer::run(){
    try{
        // Reuse NIOServer's default JDK dual-stack bind and backlog, without its
        // post-bind SO_REUSEADDR or fatal process exit (absent from HTTPServer).
        if(mPort<0||mPort>65535)throw JavaIllegalArgumentException("Port value out of range: "+std::to_string(mPort));
        Descriptor listener{::socket(AF_INET6,SOCK_STREAM,0)};bool ipv6=listener.value>=0;
        if(listener.value<0&&(errno==EAFNOSUPPORT||errno==EPROTONOSUPPORT))listener.value=::socket(AF_INET,SOCK_STREAM,0);
        if(listener.value<0)throw socketError("Server socket");
        if(ipv6){int disabled=0;if(setsockopt(listener.value,IPPROTO_IPV6,IPV6_V6ONLY,&disabled,sizeof(disabled))<0)throw socketError("Dual-stack socket");}
        int bound;if(ipv6){sockaddr_in6 address{};address.sin6_family=AF_INET6;address.sin6_addr=in6addr_any;address.sin6_port=htons(uint16_t(mPort));bound=::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
        else{sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_ANY);address.sin_port=htons(uint16_t(mPort));bound=::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
        if(bound<0){if(errno==EADDRINUSE||errno==EADDRNOTAVAIL)throw wire::BindException("Bind failed");throw socketError("Server bind");}
        if(::listen(listener.value,50)<0)throw socketError("Server listen");
        sockaddr_storage address{};socklen_t size=sizeof(address);if(getsockname(listener.value,reinterpret_cast<sockaddr*>(&address),&size)<0)throw socketError("Bound address");
        auto socket=std::shared_ptr<PosixNIOSocket>(new PosixNIOSocket(listener.release()));auto wake=std::make_shared<SocketWakeup>();
        {std::lock_guard lock(mSocketLock);mServerSocket=socket;mWakeups.push_back(wake);}
        mBoundPort=address.ss_family==AF_INET6?ntohs(reinterpret_cast<sockaddr_in6&>(address).sin6_port):ntohs(reinterpret_cast<sockaddr_in&>(address).sin_port);
        MinimaLogger::log("Server started on port : "+std::to_string(mPort));
        while(mRunning){
            pollfd fds[]={{wake->descriptor(),POLLIN,0},{socket->descriptor(),POLLIN,0}};
            int ready=::poll(fds,2,-1);if(ready<0){if(errno==EINTR)continue;throw socketError("Server poll");}if(fds[0].revents)wake->drain();if(!mRunning)break;
            if(fds[1].revents){
                std::shared_ptr<PosixNIOSocket> accepted;
                {std::lock_guard lock(mSocketLock);if(!mRunning)break;Descriptor client{::accept(socket->descriptor(),nullptr,nullptr)};if(client.value<0){if(errno==EINTR||errno==EAGAIN||errno==EWOULDBLOCK)continue;throw socketError("Server accept");}accepted=std::shared_ptr<PosixNIOSocket>(new PosixNIOSocket(client.release()));}
                auto handler=getSocketHandler(accepted);
                // Java daemon handlers own their accepted connection independently.
                auto name=unicode::decodeUTF8("Socket Handler @ "+std::to_string(getPort()));
                std::thread([handler=std::move(handler),name=std::move(name)]()mutable{NativeThread thread{std::move(name)};runNativeThread(thread,[&]{if(handler)handler();});}).detach();
            }
        }
    }catch(const wire::BindException&){MinimaLogger::log("Server @ Port "+std::to_string(mPort)+" already in use!.. restart required..");}
    catch(const wire::SocketException& error){if(mRunning)MinimaLogger::log("RPCServer : Socket Shutdown.. "+nativeExceptionText(error));}
    catch(const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
}
}
