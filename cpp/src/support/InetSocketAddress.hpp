#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
namespace minima {
// Native adapter for the JDK address operations used by Minima's peer messages.
class InetSocketAddress {
    std::optional<std::string> mHostname;
    std::vector<uint8_t> mAddress;
    std::string mScope;
    int32_t mPort=0;
    InetSocketAddress()=default;
    void resolve(const std::string& host);
public:
    explicit InetSocketAddress(int32_t port);
    InetSocketAddress(const std::optional<std::string>& host,int32_t port);
    static std::shared_ptr<InetSocketAddress> createUnresolved(const std::optional<std::string>& host,int32_t port);
    bool isUnresolved() const {return mAddress.empty();}
    int32_t getPort() const {return mPort;}
    std::string getHostAddress() const;
    std::string getHostString() const;
    std::string toString() const;
    bool equals(const InetSocketAddress& other) const;
    int32_t hashCode() const;
    std::string equalityKey() const;
};
using SocketAddressPtr=std::shared_ptr<InetSocketAddress>;
using SocketAddressList=std::shared_ptr<std::vector<SocketAddressPtr>>;
}
