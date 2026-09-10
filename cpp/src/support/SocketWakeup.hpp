#pragma once
#include "ByteIO.hpp"
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
namespace minima {
class SocketWakeup {
    struct Descriptor {int value;~Descriptor(){if(value>=0)::close(value);}int release(){int result=value;value=-1;return result;}};
    static wire::IOException socketError(const char* operation){return wire::IOException(std::string(operation)+": "+std::strerror(errno));}
    static void configurePipe(int descriptor){int flags=fcntl(descriptor,F_GETFL,0);if(flags<0||fcntl(descriptor,F_SETFL,flags|O_NONBLOCK)<0||fcntl(descriptor,F_SETFD,FD_CLOEXEC)<0)throw socketError("Selector pipe flags");}
    mutable std::mutex mLock;
    int mRead=-1,mWrite=-1;
public:
    SocketWakeup(){int descriptors[2];if(pipe(descriptors)<0)throw socketError("Selector pipe");Descriptor read{descriptors[0]},write{descriptors[1]};configurePipe(read.value);configurePipe(write.value);mRead=read.release();mWrite=write.release();}
    ~SocketWakeup(){close();}
    int descriptor() const {std::lock_guard lock(mLock);return mRead;}
    void wake(){std::lock_guard lock(mLock);if(mWrite<0)return;uint8_t byte=1;ssize_t result;do{result=::write(mWrite,&byte,1);}while(result<0&&errno==EINTR);if(result<0&&errno!=EAGAIN&&errno!=EWOULDBLOCK)throw socketError("Selector wakeup");}
    void drain(){std::lock_guard lock(mLock);if(mRead<0)return;uint8_t bytes[1024];for(;;){auto count=::read(mRead,bytes,sizeof(bytes));if(count>0)continue;if(count<0&&errno==EINTR)continue;if(count<0&&errno!=EAGAIN&&errno!=EWOULDBLOCK)throw socketError("Selector drain");break;}}
    void close(){std::lock_guard lock(mLock);if(mRead>=0)::close(mRead);if(mWrite>=0)::close(mWrite);mRead=mWrite=-1;}
};
}
