#pragma once
#include "JavaStringSet.hpp"
#include "ByteIO.hpp"
namespace minima::p2p {
class InterfaceSocketException : public wire::IOException {public:using wire::IOException::IOException;};
std::shared_ptr<JavaStringSet> localInterfaceHostnames();
struct HostInterface {
    std::string name;
    bool loopback=false,up=false;
    std::vector<std::string> addresses;
};
// IPv4 interface/address order used by the BSD JDK NetworkInterface enumeration.
std::vector<HostInterface> localHostInterfaces();
// Native HTTP(S) counterpart of opening the response stream and closing it.
// Kept separate so loopback fixtures can exercise the real transport offline.
void openURLAndClose(const std::string& url);
}
