#pragma once
#include "org/minima/objects/base/MiniData.hpp"
#include <memory>
namespace minima {
class TimedRequest {
public:
    struct RequestUnit {
        int64_t mTimeMilli;
        std::shared_ptr<MiniData> mDataID;
        explicit RequestUnit(std::shared_ptr<MiniData> data);
    };
    std::shared_ptr<std::vector<std::shared_ptr<RequestUnit>>> mPreviousRequests;
    TimedRequest();
    bool sendNow(const std::shared_ptr<MiniData>& dataID) const;
};
}
