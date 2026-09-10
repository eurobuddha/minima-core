#include "NIOServer.hpp"
#include "support/SocketWakeup.hpp"
#include "support/InetSocketAddress.hpp"
#include "support/NativeThrowable.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <algorithm>
namespace minima {
std::atomic<bool> NIOServer::mTraceON{false};
namespace {
struct Descriptor {
    int value=-1;
    explicit Descriptor(int descriptor):value(descriptor){}
    Descriptor(const Descriptor&)=delete;
    Descriptor& operator=(const Descriptor&)=delete;
    ~Descriptor(){if(value>=0)::close(value);}
    int release(){int result=value;value=-1;return result;}
};
wire::IOException socketError(const char* operation){return wire::IOException(std::string(operation)+": "+std::strerror(errno));}
std::pair<std::string,int32_t> remoteAddress(int descriptor){
    sockaddr_storage storage{};socklen_t size=sizeof(storage);if(getpeername(descriptor,reinterpret_cast<sockaddr*>(&storage),&size)<0)throw socketError("Peer address");char buffer[INET6_ADDRSTRLEN]{};int32_t port;std::string host;
    if(storage.ss_family==AF_INET){auto& address=reinterpret_cast<sockaddr_in&>(storage);if(!inet_ntop(AF_INET,&address.sin_addr,buffer,sizeof(buffer)))throw socketError("IPv4 address");host=buffer;port=ntohs(address.sin_port);}
    else if(storage.ss_family==AF_INET6){auto& address=reinterpret_cast<sockaddr_in6&>(storage);if(!inet_ntop(AF_INET6,&address.sin6_addr,buffer,sizeof(buffer)))throw socketError("IPv6 address");host=buffer;if(address.sin6_scope_id)host+='%'+std::to_string(address.sin6_scope_id);port=ntohs(address.sin6_port);}
    else throw wire::IOException("Unsupported socket address family");
    return {InetSocketAddress(host,port).getHostAddress(),port};
}
}
NIOServer::~NIOServer()=default;
void NIOServer::wakeup(){auto selector=this->selector();if(!selector)throw std::runtime_error("Null selector");selector->wake();}
void NIOServer::shutdown(){mShutDown=true;if(auto selector=this->selector())selector->wake();}
void NIOServer::post(std::shared_ptr<Message> message){mPostMessage(std::move(message));}
void NIOServer::regsiterNewSocket(std::shared_ptr<PosixNIOSocket> channel){{std::lock_guard lock(mRegisterLock);mRegisterChannels.push_back(std::move(channel));}wakeup();}
void NIOServer::disconnect(std::optional<std::string> uid){{std::lock_guard lock(mDisconnectLock);mDisconnectChannels.push_back(std::move(uid));}wakeup();}
void NIOServer::sendMessage(const std::optional<std::string>& uid,const std::shared_ptr<MiniData>& data){auto client=getClient(uid);if(client)try{client->sendData(data);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}}
void NIOServer::sendMessageAll(const std::shared_ptr<MiniData>& data){for(const auto& client:mClients.elements())if(client&&client->isValidGreeting())try{client->sendData(data);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}}
void NIOServer::addChannel(bool incoming,const std::shared_ptr<PosixNIOSocket>& socket){
    if(!socket)throw std::runtime_error("Null registered channel");auto [host,port]=remoteAddress(socket->descriptor());int enabled=1;
    if(setsockopt(socket->descriptor(),IPPROTO_TCP,TCP_NODELAY,&enabled,sizeof(enabled))<0||setsockopt(socket->descriptor(),SOL_SOCKET,SO_KEEPALIVE,&enabled,sizeof(enabled))<0)throw socketError("Client socket options");
    auto selector=this->selector();if(!selector)throw std::runtime_error("Null selector");auto key=std::make_shared<NativeNIOSelectionKey>([selector]{selector->wake();});key->interestOps(NIOSelectionKey::OP_READ|NIOSelectionKey::OP_WRITE);key->interestOps(NIOSelectionKey::OP_READ);
    auto client=std::make_shared<NIOClient>(incoming,host,port,socket,key);mClients.put(client->getUID().value(),client);mRegistrations.push_back(std::make_shared<Registration>(Registration{client,socket,key}));
    if(mTraceON)MinimaLogger::log("[NIOSERVER] NEW NIOClient:"+client->getUID().value()+" total:"+std::to_string(mClients.size()),false);auto message=std::make_shared<Message>("NIO_NEWCONNECT");message->addObject("client",MessageObject(client));post(message);
}
void NIOServer::removeRegistration(const std::shared_ptr<NIOClient>& client){mRegistrations.erase(std::remove_if(mRegistrations.begin(),mRegistrations.end(),[&](const auto& entry){return entry->client==client;}),mRegistrations.end());}
void NIOServer::run(){
    try{
        mIsRunning=true;
        // Default JDK wildcard sockets are dual-stack when IPv6 is available.
        Descriptor listener{::socket(AF_INET6,SOCK_STREAM,0)};bool ipv6=listener.value>=0;
        if(listener.value<0&&(errno==EAFNOSUPPORT||errno==EPROTONOSUPPORT))listener.value=::socket(AF_INET,SOCK_STREAM,0);
        if(listener.value<0)throw socketError("Server socket");
        if(ipv6){int disabled=0;if(setsockopt(listener.value,IPPROTO_IPV6,IPV6_V6ONLY,&disabled,sizeof(disabled))<0)throw socketError("Dual-stack socket");}
        int flags=fcntl(listener.value,F_GETFL,0);if(flags<0||fcntl(listener.value,F_SETFL,flags|O_NONBLOCK)<0)throw socketError("Nonblocking listener");
        if(mPort<0||mPort>65535)throw JavaIllegalArgumentException("Port value out of range: "+std::to_string(mPort));
        int bound;if(ipv6){sockaddr_in6 address{};address.sin6_family=AF_INET6;address.sin6_addr=in6addr_any;address.sin6_port=htons(uint16_t(mPort));bound=::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
        else{sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_ANY);address.sin_port=htons(uint16_t(mPort));bound=::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
        // JDK ServerSocket.bind includes listen in the fatal IOException block.
        if(bound==0&&::listen(listener.value,50)<0)bound=-1;
        sockaddr_storage boundAddress{};socklen_t length=sizeof(boundAddress);
        if(bound==0&&getsockname(listener.value,reinterpret_cast<sockaddr*>(&boundAddress),&length)<0)bound=-1;
        if(bound<0){MinimaLogger::log("[!] NIO ERROR - MAIN PORT "+std::to_string(mPort)+" ALREADY IN USE. SHUTTING DOWN");std::_Exit(0);}
        // ServerSocket.bind(address) uses its default backlog (50); reuse is set
        // after binding in the pinned class, not before it as in the sibling.
        int enabled=1;if(setsockopt(listener.value,SOL_SOCKET,SO_REUSEADDR,&enabled,sizeof(enabled))<0)throw socketError("Server reuse address");
        mBoundPort=boundAddress.ss_family==AF_INET6?ntohs(reinterpret_cast<sockaddr_in6&>(boundAddress).sin6_port):ntohs(reinterpret_cast<sockaddr_in&>(boundAddress).sin_port);
        auto selector=std::make_shared<Wakeup>();{std::lock_guard lock(mSelectorLock);mSelector=selector;}post(std::make_shared<Message>("NIO_SERVERSTARTED"));
        while(!mShutDown){
            auto registrations=mRegistrations;std::vector<pollfd> fds{{selector->descriptor(),POLLIN,0},{listener.value,POLLIN,0}};
            for(const auto& entry:registrations){
                short events=0;int descriptor=-1;
                // An external disconnect may cancel between the validity check
                // and interest lookup. A cancelled key is excluded from select.
                if(entry->key->isValid())try{int interest=entry->key->interestOps();if(interest&NIOSelectionKey::OP_READ)events|=POLLIN;if(interest&NIOSelectionKey::OP_WRITE)events|=POLLOUT;descriptor=entry->socket->descriptor();}catch(const JavaError&){throw;}catch(const std::runtime_error&){if(entry->key->isValid())throw;}
                fds.push_back({descriptor,events,0});
            }
            int ready=::poll(fds.data(),nfds_t(fds.size()),30000);if(ready<0){if(errno==EINTR)continue;throw socketError("Selector poll");}if(fds[0].revents)selector->drain();
            {std::lock_guard lock(mRegisterLock);for(const auto& channel:mRegisterChannels)addChannel(false,channel);mRegisterChannels.clear();}
            {std::lock_guard lock(mDisconnectLock);for(const auto& uid:mDisconnectChannels){auto client=getClient(uid);if(client){client->disconnect();mClients.remove(uid.value());removeRegistration(client);auto message=std::make_shared<Message>("NIO_DISCONNECTED");message->addObject("client",MessageObject(client)).addBoolean("reconnect",false);post(message);}}mDisconnectChannels.clear();}
            if(fds[1].revents&POLLIN)try{Descriptor accepted{::accept(listener.value,nullptr,nullptr)};if(accepted.value<0)throw socketError("Server accept");auto socket=std::shared_ptr<PosixNIOSocket>(new PosixNIOSocket(accepted.release()));addChannel(true,socket);}catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception&){MinimaLogger::log("[NIOSERVER] NULL Client disconnect in NIOServer.. ignoring..");}
            for(size_t i=0;i<registrations.size();++i){auto entry=registrations[i];short events=fds[i+2].revents;if(!events||!entry->key->isValid())continue;try{
                if(events&(POLLIN|POLLHUP|POLLERR|POLLNVAL))entry->client->handleRead();
                if(!entry->key->isValid())throw std::runtime_error("CancelledKeyException");
                if(events&POLLOUT)entry->client->handleWrite();
            }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){entry->client->disconnect();mClients.remove(entry->client->getUID().value());removeRegistration(entry->client);if(mTraceON)MinimaLogger::log("[NIOSERVER] NIOClient:"+entry->client->getUID().value()+" "+nativeExceptionText(error)+" total:"+std::to_string(mClients.size()),false);auto message=std::make_shared<Message>("NIO_DISCONNECTED");message->addObject("client",MessageObject(entry->client)).addBoolean("reconnect",entry->client->isOutgoing());post(message);}}
        }
        ::close(listener.release());for(const auto& client:mClients.elements())if(client)client->disconnect();selector->drain();selector->close();
    }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    mIsRunning=false;
}
}
