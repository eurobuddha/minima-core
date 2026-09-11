#pragma once
#include "org/minima/system/network/minima/NIOClientInfo.hpp"
#include "support/JSONSequence.hpp"
namespace minima::p2p::testing {
class QuickClients {
public:
    using Clients=json::JSONSequence<std::shared_ptr<NIOClientInfo>,true>;
    static std::shared_ptr<Clients> generateClientInfoList(const std::optional<std::string>& prefix,int count,int port,const std::optional<std::string>& uidPrefix,bool incoming);
};
}
