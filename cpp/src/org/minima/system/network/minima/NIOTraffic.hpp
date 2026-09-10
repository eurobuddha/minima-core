#pragma once
#include "support/JavaHashtable.hpp"
#include "org/minima/utils/MiniFormat.hpp"
#include <mutex>
namespace minima {
class NIOTraffic {
    std::mutex mLock;
    int64_t mStartTime=0,mTotalRead=0,mTotalWrite=0;
    JavaHashtable<int64_t> mReadBreakDown,mWriteBreakDown;
public:
    NIOTraffic(){reset();}
    void reset();
    void resetAt(int64_t now);
    int64_t getStartTime();
    int64_t getTotalRead();
    int64_t getTotalWrite();
    json::JSONObject getBreakdown();
    void addToTotalRead(int64_t amount);
    void addToTotalWrite(int64_t amount);
    void addReadBytes(const std::optional<std::string>& from,int64_t amount);
    void addWriteBytes(const std::optional<std::string>& from,int64_t amount);
};
}
