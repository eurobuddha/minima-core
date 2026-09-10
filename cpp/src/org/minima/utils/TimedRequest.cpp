#include "TimedRequest.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
TimedRequest::RequestUnit::RequestUnit(std::shared_ptr<MiniData> data)
    :mTimeMilli(TimerMessage::currentTimeMillis()),mDataID(std::move(data)){}
TimedRequest::TimedRequest():mPreviousRequests(std::make_shared<std::vector<std::shared_ptr<RequestUnit>>>()){}
bool TimedRequest::sendNow(const std::shared_ptr<MiniData>& dataID) const {
    if(!mPreviousRequests) throw std::runtime_error("Null previous request list");
    for(const auto& unit:*mPreviousRequests) {
        if(!unit||!unit->mDataID||!dataID) throw std::runtime_error("Null request or data ID");
        if(unit->mDataID->isEqual(*dataID)) return false;
    }
    return true;
}
}
