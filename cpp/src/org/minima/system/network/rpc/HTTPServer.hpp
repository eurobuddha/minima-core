#pragma once
#include "Server.hpp"
#include "support/NIOSocket.hpp"
#include "support/SocketWakeup.hpp"
#include <thread>
#include <vector>
namespace minima {
class HTTPServer : public Server,public std::enable_shared_from_this<HTTPServer> {
    std::mutex mSocketLock,mThreadLock;
    std::shared_ptr<PosixNIOSocket> mServerSocket;
    std::vector<std::weak_ptr<SocketWakeup>> mWakeups;
    std::vector<std::thread> mThreads;
    std::atomic<int32_t> mBoundPort{0};
protected:
    std::atomic<bool> mRunning{true};
    // As for MessageProcessor, start only after derived shared ownership exists.
    explicit HTTPServer(int32_t port):Server(port){}
public:
    ~HTTPServer() override;
    void shutdown() override;
    void start();
    void join();
    virtual void run();
    virtual std::function<void()> getSocketHandler(const std::shared_ptr<PosixNIOSocket>& socket)=0;
    int32_t getBoundPort() const{return mBoundPort;}
};
}
