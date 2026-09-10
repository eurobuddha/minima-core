#pragma once
#include "TimerMessage.hpp"
#include <atomic>
#include <condition_variable>
#include <thread>
namespace minima {
class TimerProcessor {
    std::atomic<bool> mRunning{true};
    std::thread mMainThread;
    std::vector<std::shared_ptr<TimerMessage>> mTimerMessages;
    mutable std::mutex mMessagesLock;
    std::mutex mSleepLock,mJoinLock;
    std::condition_variable mSleep;
    TimerProcessor()=default;
public:
    ~TimerProcessor();
    static void createTimerProcessor();
    static std::shared_ptr<TimerProcessor> getTimerProcessor();
    static void stopTimerProcessor();
    // Native deterministic polling adapter; the regular factory runs the
    // same poll once per second, using system time and a strict deadline.
    static std::shared_ptr<TimerProcessor> createManual(){return std::shared_ptr<TimerProcessor>(new TimerProcessor);}
    void stop();
    void join();
    void PostMessage(std::shared_ptr<TimerMessage> message);
    int32_t getSize() const;
    void printAllMessages() const;
    void pollAt(int64_t time);
    void run();
};
}
