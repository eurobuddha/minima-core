#pragma once
#include "NativeThread.hpp"
#include "JavaUnicode.hpp"
#include <atomic>
#include <bit>
#include <exception>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <algorithm>
#include "JavaExceptions.hpp"
namespace minima {
// Executors.newFixedThreadPool boundary: lazy workers, unbounded FIFO queue,
// shutdown drains work, and a timed await does not cancel remaining tasks.
// Condition-variable queue adapts the inspected MessageProcessor implementations.
class FixedTaskExecutor {
    friend struct FixedTaskExecutorFixture;
    inline static std::atomic<uint32_t> sPoolNumber{1};
    struct TerminationWait {
        enum class Outcome {Waiting,Signalled,Interrupted,TimedOut};
        std::mutex lock;
        std::condition_variable changed;
        Outcome outcome=Outcome::Waiting;
        void resolve(Outcome value){
            {std::lock_guard guard(lock);if(outcome!=Outcome::Waiting)return;outcome=value;}
            changed.notify_one();
        }
        Outcome await(int64_t nanos){
            std::unique_lock guard(lock);
            while(outcome==Outcome::Waiting&&nanos>0){
                // TimeUnit.MILLISECONDS.toNanos saturates at signed-long max.
                // Bounded native waits avoid overflowing an absolute deadline.
                auto chunk=std::min(nanos,int64_t(86400000000000));
                auto start=std::chrono::steady_clock::now();
                if(changed.wait_for(guard,std::chrono::nanoseconds(chunk),[&]{return outcome!=Outcome::Waiting;}))break;
                auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count();
                nanos-=std::min(nanos,int64_t(elapsed));
            }
            if(outcome==Outcome::Waiting)outcome=Outcome::TimedOut;
            return outcome;
        }
    };
    struct State {
        std::mutex lock;
        std::condition_variable changed;
        std::deque<std::function<void()>> queue;
        size_t workers=0,limit;
        bool shutdown=false;
        std::string namePrefix;
        uint32_t threadNumber=1;
        std::vector<std::weak_ptr<TerminationWait>> terminationWaiters;
        explicit State(size_t count):limit(count),namePrefix("pool-"+std::to_string(std::bit_cast<int32_t>(sPoolNumber.fetch_add(1)))+"-thread-"){}
        bool terminated() const{return shutdown&&workers==0;}
        // Caller owns lock. A waiter has one outcome, so a later interrupt
        // cannot turn an already delivered termination signal into an exception.
        void signalTermination(){if(terminated()){for(auto& weak:terminationWaiters)if(auto waiter=weak.lock())waiter->resolve(TerminationWait::Outcome::Signalled);terminationWaiters.clear();}}
    };
    std::shared_ptr<State> mState;
    static void spawn(const std::shared_ptr<State>& state,std::function<void()> first={}){
        // Called under state->lock; threads retain state independently of owner.
        ++state->workers;
        try{
            auto name=unicode::decodeUTF8(state->namePrefix+std::to_string(std::bit_cast<int32_t>(state->threadNumber++)));
            std::thread([state,first=std::move(first),name=std::move(name)]()mutable{
                NativeThread thread{std::move(name)};
                runNativeThread(thread,[&]{
                    std::exception_ptr failure;
                    try{
                        for(;;){
                            if(!first){std::unique_lock lock(state->lock);state->changed.wait(lock,[&]{return state->shutdown||!state->queue.empty();});if(state->queue.empty())break;first=std::move(state->queue.front());state->queue.pop_front();}
                            first();first={};
                        }
                    }catch(...){failure=std::current_exception();}
                    // runWorker's finally clears the task, then processWorkerExit
                    // removes/replaces the worker BEFORE Thread dispatches it.
                    first={};
                    {
                        std::lock_guard lock(state->lock);--state->workers;
                        if(failure&&(!state->shutdown||!state->queue.empty()))try{spawn(state);}catch(...){failure=std::current_exception();}
                        state->signalTermination();
                    }
                    state->changed.notify_all();
                    if(failure)std::rethrow_exception(failure);
                });
            }).detach();
        }catch(...){--state->workers;state->changed.notify_all();throw;}
    }
public:
    explicit FixedTaskExecutor(size_t count):mState(std::make_shared<State>(count)){if(!count)throw std::invalid_argument("Zero executor workers");}
    FixedTaskExecutor(const FixedTaskExecutor&)=delete;
    ~FixedTaskExecutor(){shutdown();}
    void execute(std::function<void()> task){if(!task)throw std::invalid_argument("Null executor task");std::lock_guard lock(mState->lock);if(mState->shutdown)throw std::runtime_error("RejectedExecutionException");if(mState->workers<mState->limit)spawn(mState,std::move(task));else{mState->queue.push_back(std::move(task));mState->changed.notify_one();}}
    void shutdown(){{std::lock_guard lock(mState->lock);mState->shutdown=true;mState->signalTermination();}mState->changed.notify_all();}
    bool awaitTermination(int64_t millis){
        auto state=mState;std::unique_lock lock(state->lock);
        if(state->terminated())return true;
        if(millis<=0)return false;
        if(nativeThreadInterrupted())throw JavaTypedException("java.lang.InterruptedException","");
        auto waiter=std::make_shared<TerminationWait>();
        std::erase_if(state->terminationWaiters,[](const auto& weak){return weak.expired();});
        state->terminationWaiters.push_back(waiter);
        NativeInterruptRegistration interrupt([weak=std::weak_ptr(waiter)]{if(auto value=weak.lock())value->resolve(TerminationWait::Outcome::Interrupted);});
        auto nanos=millis>INT64_MAX/1000000?INT64_MAX:millis*1000000;
        lock.unlock();auto outcome=waiter->await(nanos);
        // Condition.awaitNanos consumes the triggering interrupt before its
        // uninterruptible reacquisition. A later interrupt must remain pending.
        if(outcome==TerminationWait::Outcome::Interrupted)nativeThreadInterrupted();
        lock.lock();
        if(outcome==TerminationWait::Outcome::Interrupted)throw JavaTypedException("java.lang.InterruptedException","");
        // Recheck after reacquisition: termination can win after a timeout.
        // A signal-winning or late interrupt remains pending for the caller.
        return state->terminated();
    }
};
}
