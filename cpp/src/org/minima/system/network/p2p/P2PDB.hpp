#pragma once
#include "org/minima/utils/JsonDB.hpp"
#include "messages/InetSocketAddressIO.hpp"
#include "params/P2PParams.hpp"
namespace minima::p2p {
class P2PDB : public JsonDB {
public:
    P2PDB()=default;
    SocketAddressList getPeersList() const;
    void setPeersList(const SocketAddressList& peers);
    void setVersion();
    std::string getVersion() const;
};
}
