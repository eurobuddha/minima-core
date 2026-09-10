#include "org/minima/system/commands/txn/txnlock.hpp"
#include <barrier>
#include <chrono>
#include <thread>
#include <vector>
#include <stdexcept>
using minima::commands::txnlock;
static void require(bool ok) {if(!ok)throw std::runtime_error("txnlock worker assertion");}
int main() {
    constexpr int count=8;
    std::barrier begin(count+1),end(count+1);
    std::atomic<int> winners=0;
    std::vector<std::thread> threads;
    for(int i=0;i<count;++i)threads.emplace_back([&]{for(int round=0;round<100;++round){begin.arrive_and_wait();if(txnlock::lockFunction(true))++winners;end.arrive_and_wait();}});
    bool exclusive=true;
    for(int round=0;round<100;++round){txnlock::lockFunction(false);winners=0;begin.arrive_and_wait();end.arrive_and_wait();exclusive &= winners==1&&txnlock::mLocked;}
    for(auto& thread:threads)thread.join();require(exclusive);
    txnlock command;
    auto before=std::chrono::steady_clock::now();require(!command.getLock(100));
    require(std::chrono::steady_clock::now()-before>=std::chrono::milliseconds(200));require(txnlock::mLocked);
    std::thread release([]{std::this_thread::sleep_for(std::chrono::milliseconds(25));txnlock::lockFunction(false);});
    const bool obtained=command.getLock(0);release.join();require(obtained&&txnlock::mLocked);
    command.getParams()->put("action","unlock");command.getParams()->put("unlockdelay","-1");bool rejected=false;
    try{command.runCommand();}catch(const std::invalid_argument&){rejected=true;}require(rejected&&txnlock::mLocked);
    command.getParams()->put("unlockdelay","25");before=std::chrono::steady_clock::now();auto result=command.runCommand();
    require(std::chrono::steady_clock::now()-before>=std::chrono::milliseconds(25));require(!txnlock::mLocked);
    require(command.getLock(-1));command.unlock();require(!txnlock::mLocked);
}
