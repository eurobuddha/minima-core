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
namespace minima {
// Executors.newFixedThreadPool boundary: lazy workers, unbounded FIFO queue,
// shutdown drains work, and a timed await does not cancel remaining tasks.
// Condition-variable queue adapts the inspected MessageProcessor implementations.
class FixedTaskExecutor {
    inline static std::atomic<uint32_t> sPoolNumber{1};
    struct State {
        std::mutex lock;
        std::condition_variable changed;
        std::deque<std::function<void()>> queue;
        size_t workers=0,limit;
        bool shutdown=false;
        std::string namePrefix;
        uint32_t threadNumber=1;
        explicit State(size_t count):limit(count),namePrefix("pool-"+std::to_string(std::bit_cast<int32_t>(sPoolNumber.fetch_add(1)))+"-thread-"){}
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
    void shutdown(){{std::lock_guard lock(mState->lock);mState->shutdown=true;}mState->changed.notify_all();}
    bool awaitTermination(int64_t millis){std::unique_lock lock(mState->lock);return mState->changed.wait_for(lock,std::chrono::milliseconds(millis),[&]{return mState->shutdown&&mState->workers==0;});}
};
}
