#pragma once
#include "InetSocketAddress.hpp"
#include <functional>
namespace minima {
class TxPoWTreeNode;
// MinimaDB/UserDB/P2PManager boundary, following the existing ChainServices.
// The full runtime must supply these functions; missing services fail on use.
struct GreetingServices {
    std::function<void(bool)> readLock;
    std::function<std::optional<std::string>()> getWelcome;
    std::function<std::shared_ptr<TxPoWTreeNode>()> getTip;
    std::function<SocketAddressList()> getPeersCopy;
};
}
