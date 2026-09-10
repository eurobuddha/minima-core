#include "org/minima/system/network/minima/NIOServer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <thread>
#include <iostream>
using namespace minima;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
struct Descriptor {
    int value;
    explicit Descriptor(int fd):value(fd){}
    Descriptor(const Descriptor&)=delete;
    ~Descriptor(){if(value>=0)::close(value);}
    void close(){if(value>=0)::close(value);value=-1;}
    int release(){int fd=value;value=-1;return fd;}
};
struct Queue : MessageStack {
    std::shared_ptr<Message> next(const char* type){
        std::unique_lock lock(mLock);
        require(mNotify.wait_for(lock,std::chrono::seconds(5),[&]{return !mMessages.empty();}),"event timeout");
        auto message=mMessages.front();mMessages.pop_front();
        if(!message->isMessageType(type))throw std::runtime_error("unexpected event: "+message->toString());
        return message;
    }
};
static int connectTo(int port,bool ipv6=false){
    Descriptor fd(::socket(ipv6?AF_INET6:AF_INET,SOCK_STREAM,0));require(fd.value>=0,"socket");
    int status;
    if(ipv6){sockaddr_in6 address{};address.sin6_family=AF_INET6;address.sin6_addr=in6addr_loopback;address.sin6_port=htons(port);status=::connect(fd.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
    else{sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);address.sin_port=htons(port);status=::connect(fd.value,reinterpret_cast<sockaddr*>(&address),sizeof(address));}
    require(status==0,"connect");return fd.release();
}
static void writeBytes(int fd,const std::vector<uint8_t>& bytes){for(size_t offset=0;offset<bytes.size();){auto n=::send(fd,bytes.data()+offset,bytes.size()-offset,0);require(n>0,"send");offset+=size_t(n);}}
static std::vector<uint8_t> readBytes(int fd,size_t size){std::vector<uint8_t> bytes(size);for(size_t offset=0;offset<size;){pollfd p{fd,POLLIN,0};require(::poll(&p,1,5000)>0,"read timeout");auto n=::recv(fd,bytes.data()+offset,size-offset,0);require(n>0,"read EOF");offset+=size_t(n);}return bytes;}
static std::vector<uint8_t> frame(const std::vector<uint8_t>& payload){uint32_t n=uint32_t(payload.size());std::vector<uint8_t> bytes{uint8_t(n>>24),uint8_t(n>>16),uint8_t(n>>8),uint8_t(n)};bytes.insert(bytes.end(),payload.begin(),payload.end());return bytes;}
static void received(Queue& queue,const std::shared_ptr<NIOClient>& client,const std::vector<uint8_t>& payload){auto msg=queue.next("NIO_NEWMSG");require(msg->getString("uid")==client->getUID(),"message UID");require(msg->getString("fullhost")==client->getFullAddress(),"message host");require(msg->getObject("data").as<std::shared_ptr<MiniData>>()->isEqual(MiniData(payload)),"message data");}
static void disconnected(Queue& queue,const std::shared_ptr<NIOClient>& client,bool reconnect){auto msg=queue.next("NIO_DISCONNECTED");require(msg->getObject("client").as<std::shared_ptr<NIOClient>>()==client,"disconnect identity");require(msg->getBoolean("reconnect")==reconnect,"reconnect flag");}
int main(int argc,char** argv){try{
    if(argc==3){auto queue=std::make_shared<Queue>();NIOServer server(std::stoi(argv[2]),queue);bool early=std::string(argv[1])=="pre-shutdown";if(early)server.shutdown();server.run();if(early){queue->next("NIO_SERVERSTARTED");require(queue->getSize()==0&&server.getNetClientSize()==0,"pre-shutdown state");}require(!server.isRunning(),"failed startup running");std::cout<<"returned\n";return 0;}
    auto queue=std::make_shared<Queue>();auto runtime=std::make_shared<NIORuntime>();runtime->traffic=std::make_shared<NIOTraffic>();runtime->postMessage=[queue](auto msg){queue->PostMessage(msg);};NIOClient::runtimeLookup=[runtime]{return runtime;};
    struct Reset{~Reset(){NIOClient::runtimeLookup={};}}reset;
    NIOServer before(0,queue);require(!before.isRunning()&&before.getNetClientSize()==0,"initial state");
    bool failed=false;try{before.disconnect("missing");}catch(...){failed=true;}require(failed,"prestart selector exception");before.shutdown();
    NIOServer server(0,queue);std::thread worker([&]{server.run();});struct Stop{NIOServer& server;std::thread& worker;~Stop(){server.shutdown();if(worker.joinable())worker.join();}}stop{server,worker};
    queue->next("NIO_SERVERSTARTED");require(server.isRunning(),"running");std::cout<<"started\n";
    Descriptor ipv4(connectTo(server.getBoundPort()));auto c4=queue->next("NIO_NEWCONNECT")->getObject("client").as<std::shared_ptr<NIOClient>>();require(c4->isIncoming()&&c4->getHost()=="127.0.0.1","IPv4 metadata");require(server.getClient(c4->getUID())==c4,"map identity");std::cout<<"incoming4\n";
    Descriptor ipv6(connectTo(server.getBoundPort(),true));auto c6=queue->next("NIO_NEWCONNECT")->getObject("client").as<std::shared_ptr<NIOClient>>();require(c6->isIncoming()&&c6->getHost()=="0:0:0:0:0:0:0:1","IPv6 metadata");std::cout<<"incoming6\n";
    // Outgoing registration uses a separate local listener, as the sibling fixture.
    Descriptor listener(::socket(AF_INET,SOCK_STREAM,0));sockaddr_in address{};address.sin_family=AF_INET;address.sin_addr.s_addr=htonl(INADDR_LOOPBACK);require(::bind(listener.value,reinterpret_cast<sockaddr*>(&address),sizeof(address))==0,"outgoing bind");require(::listen(listener.value,1)==0,"outgoing listen");socklen_t size=sizeof(address);require(::getsockname(listener.value,reinterpret_cast<sockaddr*>(&address),&size)==0,"outgoing address");
    Descriptor connecting(connectTo(ntohs(address.sin_port))),peer(::accept(listener.value,nullptr,nullptr));require(peer.value>=0,"outgoing accept");auto channel=std::shared_ptr<PosixNIOSocket>(new PosixNIOSocket(connecting.release()));server.regsiterNewSocket(channel);auto outgoing=queue->next("NIO_NEWCONNECT")->getObject("client").as<std::shared_ptr<NIOClient>>();require(outgoing->isOutgoing()&&outgoing->getPort()==ntohs(address.sin_port),"outgoing metadata");require(server.getAllNIOClients().size()==3,"client count");std::cout<<"outgoing\n";
    int64_t totalRead=0,totalWrite=0;
    for(size_t n:{size_t(0),size_t(1),size_t(8193),size_t(65536)}){
        std::vector<uint8_t> payload(n);for(size_t i=0;i<n;++i)payload[i]=uint8_t(i*37+n);auto bytes=frame(payload);
        // Header fragments go through the real selector, not direct handleRead calls.
        for(size_t i=0;i<4;++i)writeBytes(ipv4.value,{bytes[i]});writeBytes(ipv4.value,payload);received(*queue,c4,payload);totalRead+=bytes.size();
        server.sendMessage(c6->getUID(),std::make_shared<MiniData>(payload));require(readBytes(ipv6.value,bytes.size())==bytes,"direct send");totalWrite+=bytes.size();std::cout<<"frame:"<<n<<'\n';
    }
    c4->setValidGreeting(true);outgoing->setValidGreeting(true);auto broadcast=frame({4,5,6});server.sendMessageAll(std::make_shared<MiniData>(std::vector<uint8_t>{4,5,6}));require(readBytes(ipv4.value,broadcast.size())==broadcast&&readBytes(peer.value,broadcast.size())==broadcast,"broadcast");pollfd quiet{ipv6.value,POLLIN,0};require(::poll(&quiet,1,100)==0,"invalid greeting received broadcast");totalWrite+=2*broadcast.size();std::cout<<"broadcast-filter\n";
    server.sendMessage("missing",nullptr);server.disconnect("missing");server.disconnect(c6->getUID());disconnected(*queue,c6,false);require(!server.getClient(c6->getUID())&&server.getNetClientSize()==2,"explicit remove");std::cout<<"explicit-disconnect\n";
    peer.close();disconnected(*queue,outgoing,true);require(server.getNetClientSize()==1,"outgoing remove");std::cout<<"outgoing-eof\n";
    writeBytes(ipv4.value,{0x10,0,0,1});totalRead+=4;disconnected(*queue,c4,false);require(server.getNetClientSize()==0,"invalid frame remove");std::cout<<"oversized-frame\n";
    Descriptor finalPeer(connectTo(server.getBoundPort()));auto finalClient=queue->next("NIO_NEWCONNECT")->getObject("client").as<std::shared_ptr<NIOClient>>();server.shutdown();worker.join();require(!server.isRunning()&&server.getNetClientSize()==1&&server.getClient(finalClient->getUID())==finalClient,"shutdown retained map");pollfd closed{finalPeer.value,POLLIN,0};require(::poll(&closed,1,5000)>0,"shutdown EOF wait");uint8_t byte;require(::recv(finalPeer.value,&byte,1,0)==0,"shutdown EOF");require(queue->getSize()==0,"unexpected shutdown event");require(runtime->traffic->getTotalRead()==totalRead&&runtime->traffic->getTotalWrite()==totalWrite,"traffic totals");std::cout<<"shutdown-retains-map\ntraffic:"<<totalRead<<':'<<totalWrite<<'\n';
}catch(const std::exception& error){std::cerr<<error.what()<<" (errno="<<errno<<")\n";return 1;}}
