#include "org/minima/system/network/minima/NIOClient.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <iostream>
using namespace minima;
static void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
struct Descriptor {int value=-1;~Descriptor(){if(value>=0)::close(value);}int release(){int old=value;value=-1;return old;}};
struct CountedSocket final : NIOSocket {
    std::shared_ptr<PosixNIOSocket> socket;
    int blockedWrites=0,partialWrites=0;
    explicit CountedSocket(int descriptor):socket(std::make_shared<PosixNIOSocket>(descriptor)){}
    int32_t read(std::span<uint8_t> buffer) override{return socket->read(buffer);}
    int32_t write(std::span<const uint8_t> buffer) override{auto count=socket->write(buffer);if(!buffer.empty()&&count==0)++blockedWrites;if(count>0&&size_t(count)<buffer.size())++partialWrites;return count;}
    void close() override{socket->close();}
};
int main(){
    try {
        // Reuse the sibling loopback fixture, binding port zero atomically so
        // there is no free-port probe/close race and no remote network access.
        Descriptor listener{::socket(AF_INET,SOCK_STREAM,0)};require(listener.value>=0,"socket listener failed");sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);address.sin_port=0;
        require(::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address))==0,"loopback bind failed");require(::listen(listener.value,1)==0,"listen failed");socklen_t length=sizeof(address);require(::getsockname(listener.value,reinterpret_cast<sockaddr*>(&address),&length)==0,"listener address failed");
        Descriptor connecting{::socket(AF_INET,SOCK_STREAM,0)};require(connecting.value>=0,"socket client failed");require(::connect(connecting.value,reinterpret_cast<sockaddr*>(&address),sizeof(address))==0,"loopback connect failed");Descriptor accepted{::accept(listener.value,nullptr,nullptr)};require(accepted.value>=0,"loopback accept failed");
        int small=4096;require(::setsockopt(connecting.value,SOL_SOCKET,SO_SNDBUF,&small,sizeof(small))==0,"send buffer failed");require(::setsockopt(accepted.value,SOL_SOCKET,SO_RCVBUF,&small,sizeof(small))==0,"receive buffer failed");
        auto left=std::make_shared<CountedSocket>(connecting.release()),right=std::make_shared<CountedSocket>(accepted.release());int leftFD=left->socket->descriptor(),rightFD=right->socket->descriptor();require((::fcntl(leftFD,F_GETFL)&O_NONBLOCK)!=0,"channel is blocking");
        std::atomic<int> wakes=0;auto leftKey=std::make_shared<NativeNIOSelectionKey>([&]{++wakes;}),rightKey=std::make_shared<NativeNIOSelectionKey>([&]{++wakes;});
        auto runtime=std::make_shared<NIORuntime>();runtime->traffic=std::make_shared<NIOTraffic>();NIOClient::runtimeLookup=[runtime]{return runtime;};struct Reset{~Reset(){NIOClient::runtimeLookup={};}}reset;
        auto sender=std::make_shared<NIOClient>(false,"127.0.0.1",ntohs(address.sin_port),left,leftKey),receiver=std::make_shared<NIOClient>(true,"127.0.0.1",ntohs(address.sin_port),right,rightKey);
        std::vector<std::shared_ptr<MiniData>> sent;for(size_t size:{size_t(0),size_t(1),size_t(8187),size_t(8192),size_t(1024*1024)}){std::vector<uint8_t> bytes(size);for(size_t i=0;i<size;++i)bytes[i]=uint8_t(i*37+size);sent.push_back(std::make_shared<MiniData>(std::move(bytes)));}
        size_t received=0,echoed=0;auto receiverUID=receiver->getUID(),senderUID=sender->getUID();std::weak_ptr<NIOClient> weakReceiver=receiver;
        runtime->postMessage=[&](const std::shared_ptr<Message>& message){require(message->isMessageType("NIO_NEWMSG"),"wrong message type");auto bytes=message->getObject("data").as<std::shared_ptr<MiniData>>();if(message->getString("uid")==receiverUID){require(received<sent.size()&&bytes->isEqual(*sent[received++]),"received payload differs");auto target=weakReceiver.lock();require(bool(target),"receiver lifetime");target->sendData(bytes);}else{require(message->getString("uid")==senderUID,"wrong sender UID");require(echoed<sent.size()&&bytes->isEqual(*sent[echoed++]),"echo payload differs");}};
        for(auto& data:sent)sender->sendData(data);require(leftKey->interestOps()==5,"write interest missing");
        // Hold off all reads to force EAGAIN on an actual nonblocking descriptor.
        for(int i=0;i<1024&&left->blockedWrites==0;++i)sender->handleWrite();require(left->blockedWrites>0,"socket backpressure was not exercised");
        auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(15);
        while(echoed<sent.size()&&std::chrono::steady_clock::now()<deadline){pollfd fds[]={{leftFD,short(POLLIN|((leftKey->interestOps()&4)?POLLOUT:0)),0},{rightFD,short(POLLIN|((rightKey->interestOps()&4)?POLLOUT:0)),0}};int ready=::poll(fds,2,50);require(ready>=0,"poll failed");if(fds[0].revents&POLLIN)sender->handleRead();if(fds[0].revents&POLLOUT)sender->handleWrite();if(fds[1].revents&POLLIN)receiver->handleRead();if(fds[1].revents&POLLOUT)receiver->handleWrite();}
        require(received==sent.size()&&echoed==sent.size(),"loopback transfer timed out");require(leftKey->interestOps()==1&&rightKey->interestOps()==1,"write interest not cleared");int64_t expected=0;for(const auto& data:sent)expected+=int64_t(data->size())+4;require(runtime->traffic->getTotalRead()==expected*2,"read traffic differs");require(runtime->traffic->getTotalWrite()==expected*2,"write traffic differs");require(wakes==int(sent.size()*2),"selector wake count differs");
        sender->disconnect();require(!leftKey->isValid(),"disconnect did not cancel key");require(::fcntl(leftFD,F_GETFD)==-1,"disconnect did not close descriptor");bool eof=false;deadline=std::chrono::steady_clock::now()+std::chrono::seconds(2);while(!eof&&std::chrono::steady_clock::now()<deadline){pollfd fd{rightFD,POLLIN,0};::poll(&fd,1,20);try{receiver->handleRead();}catch(const wire::IOException& error){eof=std::string(error.what())=="Socket Closed!";}}
        require(eof,"peer EOF not reported");receiver->disconnect();require(!rightKey->isValid(),"receiver key remains valid");std::cout<<"Five framed payloads, 1MiB transfer, actual EAGAIN, selector interests/wakeups, traffic and EOF pass\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<" (errno="<<errno<<")\n";return 1;}
}
