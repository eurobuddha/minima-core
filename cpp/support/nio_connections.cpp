#include "support/NIOConnections.hpp"
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <chrono>
#include <thread>
#include <iostream>
using namespace minima;
static volatile sig_atomic_t signals=0;
static void interrupted(int){signals=signals+1;}
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
int main(){try{
    int descriptors[2];require(::socketpair(AF_UNIX,SOCK_STREAM,0,descriptors)==0,"socketpair failed");
    auto reader=std::make_shared<PosixNIOSocket>(descriptors[0]),writer=std::make_shared<PosixNIOSocket>(descriptors[1]);
    struct sigaction action{},previous{};action.sa_handler=interrupted;sigemptyset(&action.sa_mask);require(sigaction(SIGUSR1,&action,&previous)==0,"signal handler failed");
    auto target=pthread_self();std::atomic<bool> killFailed=false;
    auto begin=std::chrono::steady_clock::now();bool timedOut=false;std::exception_ptr failure;
    std::thread interrupter([&]{for(int i=0;i<50;++i){std::this_thread::sleep_for(std::chrono::milliseconds(10));if(pthread_kill(target,SIGUSR1)!=0)killFailed=true;}});
    try{(void)nioReadAll(reader,1,80);}catch(const wire::IOException& error){timedOut=std::string(error.what())=="SocketTimeoutException";}catch(...){failure=std::current_exception();}
    auto elapsed=std::chrono::steady_clock::now()-begin;interrupter.join();int restored=sigaction(SIGUSR1,&previous,nullptr);
    if(failure)std::rethrow_exception(failure);
    require(restored==0&&!killFailed&&signals>0,"signal fixture failed");require(timedOut,"interrupted read did not time out");
    require(elapsed>=std::chrono::milliseconds(60)&&elapsed<std::chrono::milliseconds(300),"signals reset the socket deadline");
    std::cout<<"interrupted socket poll retains original deadline\n";
    std::exception_ptr producerError;
    std::thread producer([&]{try{for(uint8_t i=0;i<5;++i){std::this_thread::sleep_for(std::chrono::milliseconds(80));nioWriteAll(writer,std::span<const uint8_t>(&i,1));}}catch(...){producerError=std::current_exception();}});
    wire::Bytes data;failure=nullptr;begin=std::chrono::steady_clock::now();try{data=nioReadAll(reader,5,200);}catch(...){failure=std::current_exception();}producer.join();elapsed=std::chrono::steady_clock::now()-begin;
    if(producerError)std::rethrow_exception(producerError);if(failure)std::rethrow_exception(failure);
    require(data==wire::Bytes({0,1,2,3,4})&&elapsed>std::chrono::milliseconds(200),"partial progress did not renew per-read timeout");
    std::cout<<"partial reads renew timeout; total frame time can exceed timeout\n";
    std::thread later([&]{std::this_thread::sleep_for(std::chrono::milliseconds(20));uint8_t byte=9;try{nioWriteAll(writer,std::span<const uint8_t>(&byte,1));}catch(...){producerError=std::current_exception();}});
    failure=nullptr;try{data=nioReadAll(reader,1,0);}catch(...){failure=std::current_exception();}later.join();if(producerError)std::rethrow_exception(producerError);if(failure)std::rethrow_exception(failure);require(data==wire::Bytes({9}),"zero timeout is not infinite");
    writer->close();bool eof=false;try{(void)nioReadAll(reader,1,100);}catch(const wire::EOFException&){eof=true;}require(eof,"EOF not distinguished from timeout");
    std::cout<<"zero timeout waits; EOF is distinct\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
