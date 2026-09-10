#include "support/ReentrantReadWriteLock.hpp"
#include <atomic>
#include <barrier>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace minima;
using namespace std::chrono_literals;
int main(){
    ReentrantReadWriteLock lock;std::atomic<int> failures=0,readers=0,writers=0;int value=0;
    std::barrier start(8);std::vector<std::thread> workers;
    for(int i=0;i<8;++i)workers.emplace_back([&,i]{
        start.arrive_and_wait();for(int n=0;n<512;++n){
            if(i%2){lock.writeLock();lock.writeLock();if(++writers!=1||readers!=0)++failures;
                lock.readLock();if(lock.getWriteHoldCount()!=2||lock.getReadHoldCount()!=1)++failures;
                ++value;lock.readUnlock();--writers;lock.writeUnlock();lock.writeUnlock();
            }else{lock.readLock();lock.readLock();++readers;if(writers!=0||lock.getReadHoldCount()!=2||lock.getWriteHoldCount()!=0)++failures;
                int snapshot=value;std::this_thread::yield();if(snapshot!=value)++failures;
                --readers;lock.readUnlock();lock.readUnlock();}
        }
    });
    for(auto& worker:workers)worker.join();if(value!=2048||readers!=0||writers!=0)++failures;
    // Shared readers must be admitted while another reader still owns the lock.
    lock.readLock();std::promise<void> shared;auto entered=shared.get_future();
    std::thread reader([&]{lock.readLock();shared.set_value();lock.readUnlock();});
    bool concurrent=entered.wait_for(5s)==std::future_status::ready;lock.readUnlock();reader.join();if(!concurrent)++failures;
    // A writer can downgrade to a reader; a competing writer stays excluded.
    lock.writeLock();lock.readLock();std::promise<void> waiting,acquired;auto ready=waiting.get_future();auto got=acquired.get_future();
    std::thread writer([&]{waiting.set_value();lock.writeLock();acquired.set_value();lock.writeUnlock();});ready.wait();
    lock.writeUnlock();if(got.wait_for(30ms)==std::future_status::ready)++failures;
    lock.readUnlock();if(got.wait_for(5s)!=std::future_status::ready)++failures;writer.join();
    // Thread ownership, including nested holds, survives rejected unlocks.
    lock.writeLock();lock.readLock();std::thread intruder([&]{
        try{lock.writeUnlock();++failures;}catch(const JavaIllegalMonitorStateException& error){if(std::string(error.what())!="")++failures;}
        try{lock.readUnlock();++failures;}catch(const JavaIllegalMonitorStateException& error){if(std::string(error.what())!="attempt to unlock read lock, not locked by current thread")++failures;}
        if(lock.getReadHoldCount()!=0||lock.getWriteHoldCount()!=0)++failures;
    });intruder.join();if(lock.getReadHoldCount()!=1||lock.getWriteHoldCount()!=1)++failures;lock.readUnlock();lock.writeUnlock();
    // Recycled OS thread IDs must not inherit a terminated thread's ownership.
    ReentrantReadWriteLock abandoned;std::thread owner([&]{abandoned.writeLock();});owner.join();
    for(int i=0;i<32;++i){std::thread next([&]{if(abandoned.getWriteHoldCount()!=0)++failures;try{abandoned.writeUnlock();++failures;}catch(const JavaIllegalMonitorStateException&) {}});next.join();}
    std::cout<<"MinimaDB lock: 4096 concurrent operations, shared readers, downgrade, wrong-thread unlock and terminated-owner checks: "<<(failures==0?"PASS":"FAIL")<<'\n';return failures==0?0:1;
}
