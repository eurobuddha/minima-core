#include "txnlock.hpp"
#include "support/NativeThread.hpp"
#include <bit>
#include <chrono>
#include <thread>
namespace minima::commands {
std::mutex txnlock::mLockMutex;
std::atomic<bool> txnlock::mLocked=false;
txnlock::txnlock():Command("txnlock","(action) (timeout) (unlockdelay) - Gain a lock. Stops multiple txnfunctions occuring simultaneously"){}
std::optional<std::string> txnlock::getFullHelp() const {
    return "\txnlock\n"
        "\n"
        "When creating multiple transactions asynchronously you can ensure each is created one at a time.\n"
        "\n"
        "action:\n"
        "    lock - Gain a lock\n"
        "    unlock - Release lock\n"
        "    list/blank - Are we locked\n"
        "\n"
        "timeout:\n"
        "    How long to wait for the lock\n"
        "\n"
        "unlockdelay:\n"
        "    Wait this amount of milli after unlock.\n"
        "\n"
        "Examples:\n"
        "\n"
        "txnlock\n"
        "\n"
        "txnlock action:lock timeout:10000\n"
        "\n"
        "txnlock action:unlock unlockdelay:5000\n"
        "\n";
}
std::vector<std::string> txnlock::getValidParams() const {return {"action","timeout","unlockdelay"};}
bool txnlock::lockFunction(bool enable) {
    std::lock_guard lock(mLockMutex);
    if(enable) {if(!mLocked){mLocked=true;return true;}}
    else {mLocked=false;return true;}
    return false;
}
bool txnlock::getLock(int64_t timeout) {
    int64_t counter=0;
    while(!lockFunction(true)) {
        nativeThreadSleep(100);
        counter=std::bit_cast<int64_t>(uint64_t(counter)+100u);
        if(timeout!=0&&counter>timeout) return false;
    }
    return true;
}
void txnlock::unlock() {lockFunction(false);}
json::JSONValue txnlock::runCommand() {
    auto result=getJSONReply();auto action=getParam("action","list");
    auto timeout=getNumberParam("timeout",MiniNumber(20000)).getAsLong();
    auto delay=getNumberParam("unlockdelay",MiniNumber::ZERO).getAsLong();
    json::JSONObject response;
    if(action=="lock") {response.put("success",getLock(timeout));response.put("locked",true);}
    else if(action=="unlock") {
        if(delay!=0)nativeThreadSleep(delay);
        unlock();response.put("success",true);response.put("locked",false);
    } else response.put("locked",mLocked.load());
    result.put("response",response);return result;
}
std::shared_ptr<Command> txnlock::getFunction() const {return std::make_shared<txnlock>();}
}
