#include "NIOTraffic.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
#include <bit>
namespace minima {
static int64_t sum(int64_t a,int64_t b){return std::bit_cast<int64_t>(uint64_t(a)+uint64_t(b));}
void NIOTraffic::reset(){resetAt(TimerMessage::currentTimeMillis());}
void NIOTraffic::resetAt(int64_t now){std::lock_guard lock(mLock);mStartTime=now;mTotalRead=0;mTotalWrite=0;mReadBreakDown=JavaHashtable<int64_t>();mWriteBreakDown=JavaHashtable<int64_t>();}
int64_t NIOTraffic::getStartTime(){std::lock_guard lock(mLock);return mStartTime;}
int64_t NIOTraffic::getTotalRead(){std::lock_guard lock(mLock);return mTotalRead;}
int64_t NIOTraffic::getTotalWrite(){std::lock_guard lock(mLock);return mTotalWrite;}
json::JSONObject NIOTraffic::getBreakdown(){
    std::lock_guard lock(mLock);json::JSONObject result,reads,writes;
    for(const auto& key:mReadBreakDown.keys())reads.put(key,MiniFormat::formatSize(*mReadBreakDown.get(key)));
    for(const auto& key:mWriteBreakDown.keys())writes.put(key,MiniFormat::formatSize(*mWriteBreakDown.get(key)));
    result.put("reads",reads);result.put("writes",writes);return result;
}
void NIOTraffic::addToTotalRead(int64_t amount){std::lock_guard lock(mLock);mTotalRead=sum(mTotalRead,amount);}
void NIOTraffic::addToTotalWrite(int64_t amount){std::lock_guard lock(mLock);mTotalWrite=sum(mTotalWrite,amount);}
void NIOTraffic::addReadBytes(const std::optional<std::string>& from,int64_t amount){std::lock_guard lock(mLock);const auto& key=from.value();auto current=mReadBreakDown.get(key);mReadBreakDown.put(key,sum(current?*current:0,amount));}
void NIOTraffic::addWriteBytes(const std::optional<std::string>& from,int64_t amount){std::lock_guard lock(mLock);const auto& key=from.value();auto current=mWriteBreakDown.get(key);mWriteBreakDown.put(key,sum(current?*current:0,amount));}
}
