#include "QuickClients.hpp"
namespace minima::p2p::testing {
std::shared_ptr<QuickClients::Clients> QuickClients::generateClientInfoList(const std::optional<std::string>& prefix,int count,int port,const std::optional<std::string>& uidPrefix,bool incoming){
    auto result=std::make_shared<Clients>();
    for(int i=0;i<count;++i){
        // Java evaluates the UID argument before the host argument.
        if(!uidPrefix)throw JavaNullPointerException("null client UID prefix");
        auto uid=*uidPrefix+std::to_string(i+1);
        if(!prefix)throw JavaNullPointerException("null client host prefix");
        auto host=*prefix+std::to_string(i+1);
        result->add(std::make_shared<NIOClientInfo>(std::move(uid),std::move(host),port,incoming));
    }
    return result;
}
}
