#pragma once
#include <cstdint>
namespace minima {
class Server {
protected:
    int32_t mPort=0;
public:
    explicit Server(int32_t port);
    virtual ~Server()=default;
    int32_t getPort() const;
    virtual void shutdown()=0;
};
}
