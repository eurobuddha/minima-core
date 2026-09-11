#include "QuickInetLists.hpp"
#include "support/JavaRandom48.hpp"
namespace minima::p2p::testing {
SocketAddressList QuickInetLists::generateInetSockAddrList(const std::optional<std::string>& prefix,int count,int port){
    auto result=std::make_shared<std::vector<SocketAddressPtr>>();
    for(int i=0;i<count;++i){if(!prefix)throw JavaNullPointerException("null address prefix");result->push_back(InetSocketAddress::fromResolvedHost(*prefix+std::to_string(i+1),port));}
    return result;
}
SocketAddressList QuickInetLists::generateInetSockAddrWithHostnameList(const std::optional<std::string>& hostname,const std::optional<std::string>& prefix,int count,int port){
    if(!hostname)throw JavaNullPointerException("null hostname");auto combined=*hostname+"/";
    if(!prefix)throw JavaNullPointerException("null address prefix");combined+=*prefix;
    return generateInetSockAddrList(combined,count,port);
}
PeerMapPtr QuickInetLists::generateConnectionUIDMap(const std::optional<std::string>& uidPrefix,const std::optional<std::string>& ipPrefix,int count,int port){
    JavaRandom48 random;auto result=std::make_shared<PeerMap>();const bool randomisePort=port<=0;
    for(int i=0;i<count;++i){
        if(randomisePort)port=7000+random.nextInt(3000);
        if(!uidPrefix)throw JavaNullPointerException("null UID prefix");auto uid=*uidPrefix+std::to_string(i+1);
        if(!ipPrefix)throw JavaNullPointerException("null IP prefix");auto host=*ipPrefix+std::to_string(i+1);
        auto address=InetSocketAddress::fromResolvedHost(host,port);result->put(uid,std::move(address));
    }
    return result;
}
}
