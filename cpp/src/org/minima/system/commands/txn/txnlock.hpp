#pragma once
#include "../Command.hpp"
#include <atomic>
#include <mutex>
namespace minima::commands {
class txnlock:public Command {
    static std::mutex mLockMutex;
public:
    static std::atomic<bool> mLocked;
    txnlock();
    static bool lockFunction(bool enable);
    bool getLock(int64_t timeout);
    void unlock();
    std::optional<std::string> getFullHelp() const override;
    std::vector<std::string> getValidParams() const override;
    json::JSONValue runCommand() override;
    std::shared_ptr<Command> getFunction() const override;
};
}
