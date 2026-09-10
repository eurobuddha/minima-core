#pragma once
#include "JavaExceptions.hpp"
#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
namespace minima {
// Native mutex/condition-variable adapter for the nonfair read/write operations
// used by MinimaDB. Reentrant readers/writers and writer-to-reader downgrading
// follow the inspected JDK contract; read-to-write upgrading blocks.
class ReentrantReadWriteLock {
    struct ThreadToken {};
    using Thread=std::shared_ptr<ThreadToken>;
    static Thread current(){thread_local auto token=std::make_shared<ThreadToken>();return token;}
    struct Waiter {bool writer;};
    mutable std::mutex mMutex;
    std::condition_variable mChanged;
    Thread mWriter;
    int32_t mWrites=0,mReads=0;
    std::map<Thread,int32_t,std::owner_less<Thread>> mReaders;
    std::deque<Waiter*> mQueue;
    void await(std::unique_lock<std::mutex>& lock,bool writer,const auto& ready){
        if(ready())return;
        Waiter waiter{writer};mQueue.push_back(&waiter);
        try{mChanged.wait(lock,ready);}
        catch(...){std::erase(mQueue,&waiter);mChanged.notify_all();throw;}
        std::erase(mQueue,&waiter);mChanged.notify_all();
    }
public:
    void readLock(){
        auto thread=current();std::unique_lock lock(mMutex);
        await(lock,false,[&]{return (!mWriter||mWriter==thread)&&
            (mWriter==thread||mReaders.contains(thread)||mQueue.empty()||!mQueue.front()->writer);});
        if(mReads==INT32_MAX)throw std::overflow_error("Maximum lock count exceeded");
        ++mReaders[thread];++mReads;
    }
    void readUnlock(){
        auto thread=current();std::lock_guard lock(mMutex);auto it=mReaders.find(thread);
        if(it==mReaders.end())throw JavaIllegalMonitorStateException("attempt to unlock read lock, not locked by current thread");
        if(--it->second==0)mReaders.erase(it);--mReads;mChanged.notify_all();
    }
    void writeLock(){
        auto thread=current();std::unique_lock lock(mMutex);
        await(lock,true,[&]{return mWriter==thread||(!mWriter&&mReads==0);});
        if(mWrites==INT32_MAX)throw std::overflow_error("Maximum lock count exceeded");
        mWriter=thread;++mWrites;
    }
    void writeUnlock(){
        auto thread=current();std::lock_guard lock(mMutex);
        if(mWriter!=thread)throw JavaIllegalMonitorStateException("");
        if(--mWrites==0)mWriter.reset();mChanged.notify_all();
    }
    int32_t getWriteHoldCount() const {auto thread=current();std::lock_guard lock(mMutex);return mWriter==thread?mWrites:0;}
    int32_t getReadHoldCount() const {auto thread=current();std::lock_guard lock(mMutex);auto it=mReaders.find(thread);return it==mReaders.end()?0:it->second;}
    std::string toString() const {
        std::lock_guard lock(mMutex);std::ostringstream out;out<<"java.util.concurrent.locks.ReentrantReadWriteLock@"<<std::hex<<reinterpret_cast<uintptr_t>(this)
            <<std::dec<<"[Write locks = "<<mWrites<<", Read locks = "<<mReads<<"]";return out.str();
    }
};
}
