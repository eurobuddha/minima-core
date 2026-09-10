#include "Server.hpp"
namespace minima {
Server::Server(int32_t port):mPort(port){}
int32_t Server::getPort() const{return mPort;}
}
