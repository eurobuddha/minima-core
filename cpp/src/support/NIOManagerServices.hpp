#pragma once
#include "IBDServices.hpp"
#include "org/minima/utils/messages/Message.hpp"
namespace minima {
// Exact external receivers used by NIOManager; no default no-op behavior.
// Main/NetworkManager/P2PFunctions/NIOMessage bind these as they are ported.
struct NIOManagerServices {
    std::function<void()> releaseLocks;
    std::function<bool()> isShuttingDown,isRestoring;
    std::function<void(std::shared_ptr<Message>)> postP2P;
    std::function<bool(const std::string&)> isInvalidPeer;
    std::function<bool(const std::optional<std::string>&)> txPoWExists;
    std::function<void(const std::string&)> addInvalidPeer,clearClientSync;
    std::function<std::shared_ptr<TxPoWTreeNode>()> getRoot;
    std::function<std::shared_ptr<TxBlock>()> loadLastArchiveBlock;
    std::function<void(std::optional<std::string>,std::shared_ptr<MiniData>,bool,std::string,std::optional<std::string>)> processIncoming;
    std::function<int64_t()> currentTimeMillis;
};
}
