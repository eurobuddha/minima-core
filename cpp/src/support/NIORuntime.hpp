#pragma once
#include "org/minima/system/network/minima/NIOTraffic.hpp"
#include "org/minima/utils/messages/Message.hpp"
#include "org/minima/utils/messages/TimerMessage.hpp"
namespace minima {
// Required Main/NetworkManager binding. No implicit receiver or silent sink.
struct NIORuntime {
    std::shared_ptr<NIOTraffic> traffic;
    std::function<void(std::shared_ptr<Message>)> postMessage;
    std::function<int64_t()> currentTimeMillis=TimerMessage::currentTimeMillis;
};
}
