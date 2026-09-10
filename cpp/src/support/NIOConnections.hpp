#pragma once
#include "NIOSocket.hpp"
#include "InetSocketAddress.hpp"
namespace minima {
// Reuses the sibling POSIX connector/ownership pattern and existing JDK address
// resolution adapter. No timeout for SocketChannel.open; timed Socket.connect
// uses the explicit timeout argument and read polling is per socket operation.
std::shared_ptr<PosixNIOSocket> nioConnect(const InetSocketAddress& address,int timeoutMillis=-1);
void nioWriteAll(const std::shared_ptr<PosixNIOSocket>& socket,std::span<const uint8_t> bytes);
int32_t nioReadSome(const std::shared_ptr<PosixNIOSocket>& socket,std::span<uint8_t> bytes,int timeoutMillis=-1);
std::vector<uint8_t> nioReadAll(const std::shared_ptr<PosixNIOSocket>& socket,size_t count,int timeoutMillis);
}
