#include "NIOConnections.hpp"
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <chrono>
#include <optional>
namespace minima {
namespace {
struct Descriptor{int value;explicit Descriptor(int fd):value(fd){}Descriptor(const Descriptor&)=delete;Descriptor& operator=(const Descriptor&)=delete;~Descriptor(){if(value>=0)::close(value);}int release(){int result=value;value=-1;return result;}};
wire::IOException socketError(const char* operation){return wire::IOException(std::string(operation)+": "+std::strerror(errno));}
using Clock=std::chrono::steady_clock;
using Deadline=std::optional<Clock::time_point>;
Deadline deadline(int timeout){return timeout>0?Deadline(Clock::now()+std::chrono::milliseconds(timeout)):std::nullopt;}
void ready(int descriptor,short event,const Deadline& end){
    for(;;){
        int timeout=-1;
        if(end){auto remaining=*end-Clock::now();if(remaining<=Clock::duration::zero())throw wire::IOException("SocketTimeoutException");timeout=int(std::chrono::ceil<std::chrono::milliseconds>(remaining).count());}
        pollfd fd{descriptor,event,0};int result=::poll(&fd,1,timeout);
        if(result<0&&errno==EINTR)continue;
        if(result<0)throw socketError("Socket poll");
        if(result==0)throw wire::IOException("SocketTimeoutException");
        return;
    }
}
}
std::shared_ptr<PosixNIOSocket> nioConnect(const InetSocketAddress& address,int timeout){
    if(address.isUnresolved())throw wire::IOException("UnresolvedAddressException");
    addrinfo hints{};hints.ai_family=AF_UNSPEC;hints.ai_socktype=SOCK_STREAM;hints.ai_flags=AI_NUMERICHOST|AI_NUMERICSERV;
    addrinfo* raw=nullptr;auto host=address.getHostAddress(),port=std::to_string(address.getPort());int status=getaddrinfo(host.c_str(),port.c_str(),&hints,&raw);if(status!=0)throw wire::IOException(gai_strerror(status));std::unique_ptr<addrinfo,decltype(&freeaddrinfo)> addresses(raw,freeaddrinfo);if(!raw)throw wire::IOException("No resolved socket address");
    Descriptor fd{::socket(raw->ai_family,raw->ai_socktype,raw->ai_protocol)};if(fd.value<0)throw socketError("Connect socket");
    if(timeout>0){int flags=fcntl(fd.value,F_GETFL,0);if(flags<0||fcntl(fd.value,F_SETFL,flags|O_NONBLOCK)<0)throw socketError("Connect flags");}
    int connected;do{connected=::connect(fd.value,raw->ai_addr,raw->ai_addrlen);}while(connected<0&&errno==EINTR&&timeout<=0);
    if(connected<0){if(timeout<=0||(errno!=EINPROGRESS&&errno!=EINTR))throw socketError("Socket connect");ready(fd.value,POLLOUT,deadline(timeout));int error=0;socklen_t size=sizeof(error);if(getsockopt(fd.value,SOL_SOCKET,SO_ERROR,&error,&size)<0)throw socketError("Connect status");if(error){errno=error;throw socketError("Socket connect");}}
    return std::shared_ptr<PosixNIOSocket>(new PosixNIOSocket(fd.release()));
}
void nioWriteAll(const std::shared_ptr<PosixNIOSocket>& socket,std::span<const uint8_t> bytes){while(!bytes.empty()){auto count=socket->write(bytes);if(count>0)bytes=bytes.subspan(size_t(count));else ready(socket->descriptor(),POLLOUT,std::nullopt);}}
int32_t nioReadSome(const std::shared_ptr<PosixNIOSocket>& socket,std::span<uint8_t> bytes,int timeout){
    if(bytes.empty())return 0;auto end=deadline(timeout);
    for(;;){auto count=socket->read(bytes);if(count!=0)return count;ready(socket->descriptor(),POLLIN,end);}
}
std::vector<uint8_t> nioReadAll(const std::shared_ptr<PosixNIOSocket>& socket,size_t count,int timeout){
    std::vector<uint8_t> bytes(count);size_t offset=0;auto end=deadline(timeout);
    while(offset<count){auto read=socket->read(std::span<uint8_t>(bytes).subspan(offset));if(read<0)throw wire::EOFException("Socket EOF");
        if(read>0){offset+=size_t(read);end=deadline(timeout);}else ready(socket->descriptor(),POLLIN,end);
    }
    return bytes;
}
}
