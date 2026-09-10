#pragma once
#include "support/NIOSocket.hpp"
namespace minima {
class CMDHandler {
    std::shared_ptr<PosixNIOSocket> mSocket;
public:
    explicit CMDHandler(std::shared_ptr<PosixNIOSocket> socket):mSocket(std::move(socket)){}
    void run();
};
}
