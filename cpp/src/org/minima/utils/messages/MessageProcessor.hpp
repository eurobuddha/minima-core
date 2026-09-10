#pragma once
#include "MessageStack.hpp"
#include <atomic>
#include <thread>
namespace minima {
class TimerMessage;
class MessageProcessor : public MessageStack, public std::enable_shared_from_this<MessageProcessor> {
    std::string mName;
    std::function<void()> mReleaseLocks;
    std::thread mMainThread;
    std::mutex mJoinLock;
    std::atomic<bool> mRunning{true},mShutDownComplete{false},mExecuting{false};
    std::shared_ptr<Message> mLastMessage;
    static std::mutex sTraceLock;
    static bool sTrace;
    static std::string sTraceFilter,sLanguage;
protected:
    explicit MessageProcessor(std::string name,std::function<void()> releaseLocks);
    // Call once from the native factory, after constructing the most-derived
    // object under shared ownership. The worker retains it until run returns.
    void startMessageProcessor();
    virtual void processMessage(std::shared_ptr<Message> message)=0;
public:
    virtual ~MessageProcessor();
    static void setTrace(bool trace,std::string filter);
    static void setTraceLanguage(std::string language);
    const std::string& getName() const {return mName;}
    void setFullLogging(bool enabled,std::string filter){setTrace(enabled,std::move(filter));}
    bool isTrace() const;
    std::string getTraceFilter() const;
    std::shared_ptr<Message> getLastMessage() const;
    bool checkTraceFilter(const std::string& text) const;
    bool isRunning() const {return mRunning.load();}
    bool isShutdownComplete() const {return mShutDownComplete.load();}
    void waitToShutDown(int64_t timeout=15000);
    void stopMessageProcessor();
    // Native ownership barrier: wait for actual worker exit, even if the Java
    // compatible shutdown flag was set early by waitToShutDown's timeout.
    void joinMessageProcessor();
    virtual void PostTimerMessage(const std::shared_ptr<TimerMessage>& message);
    void run();
};
}
