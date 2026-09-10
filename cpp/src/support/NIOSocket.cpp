#include "NIOSocket.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <climits>
#include <algorithm>
namespace minima {
namespace {wire::IOException ioError(const char* operation){return wire::IOException(std::string(operation)+": "+std::strerror(errno));}}
PosixNIOSocket::PosixNIOSocket(int descriptor):mDescriptor(descriptor) {
    if(descriptor<0)throw std::invalid_argument("Negative socket descriptor");
    int flags=::fcntl(descriptor,F_GETFL,0);
    if(flags<0||::fcntl(descriptor,F_SETFL,flags|O_NONBLOCK)<0){auto error=ioError("Nonblocking socket");::close(mDescriptor);mDescriptor=-1;throw error;}
#ifdef SO_NOSIGPIPE
    int enabled=1;if(::setsockopt(descriptor,SOL_SOCKET,SO_NOSIGPIPE,&enabled,sizeof(enabled))<0){auto error=ioError("Socket SIGPIPE option");::close(mDescriptor);mDescriptor=-1;throw error;}
#endif
}
PosixNIOSocket::~PosixNIOSocket(){try{close();}catch(...) {}}
int32_t PosixNIOSocket::read(std::span<uint8_t> destination) {
    std::lock_guard lock(mIOLock);
    if(mDescriptor<0)throw wire::IOException("ClosedChannelException");if(destination.empty())return 0;
    ssize_t count;do{count=::recv(mDescriptor,destination.data(),std::min(destination.size(),size_t(INT32_MAX)),0);}while(count<0&&errno==EINTR);
    if(count==0)return -1;if(count<0){if(errno==EAGAIN||errno==EWOULDBLOCK)return 0;throw ioError("Socket read");}return int32_t(count);
}
int32_t PosixNIOSocket::write(std::span<const uint8_t> source) {
    std::lock_guard lock(mIOLock);
    if(mDescriptor<0)throw wire::IOException("ClosedChannelException");if(source.empty())return 0;
    int flags=0;
#ifdef MSG_NOSIGNAL
    flags=MSG_NOSIGNAL;
#endif
    ssize_t count;do{count=::send(mDescriptor,source.data(),std::min(source.size(),size_t(INT32_MAX)),flags);}while(count<0&&errno==EINTR);
    if(count<0){if(errno==EAGAIN||errno==EWOULDBLOCK)return 0;throw ioError("Socket write");}return int32_t(count);
}
void PosixNIOSocket::close() {std::lock_guard lock(mIOLock);if(mDescriptor>=0){int descriptor=mDescriptor;mDescriptor=-1;if(::close(descriptor)<0)throw ioError("Socket close");}}
}
