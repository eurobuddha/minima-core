#include "P2PEnvironment.hpp"
#include "InetSocketAddress.hpp"
#include "CurlTransport.hpp"
#include <curl/curl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <cstring>
#include <charconv>
#include <algorithm>
namespace minima::p2p {
namespace {
bool sameAddress(const sockaddr* a,const sockaddr* b){
    if(a->sa_family!=b->sa_family)return false;
    if(a->sa_family==AF_INET)return std::memcmp(&reinterpret_cast<const sockaddr_in*>(a)->sin_addr,&reinterpret_cast<const sockaddr_in*>(b)->sin_addr,4)==0;
    return std::memcmp(&reinterpret_cast<const sockaddr_in6*>(a)->sin6_addr,&reinterpret_cast<const sockaddr_in6*>(b)->sin6_addr,16)==0;
}
std::string numeric(const sockaddr* address,const char* name){
    char text[INET6_ADDRSTRLEN];bool v6=address->sa_family==AF_INET6;
    const void* bytes=v6?static_cast<const void*>(&reinterpret_cast<const sockaddr_in6*>(address)->sin6_addr):static_cast<const void*>(&reinterpret_cast<const sockaddr_in*>(address)->sin_addr);
    if(!inet_ntop(address->sa_family,bytes,text,sizeof(text)))throw InterfaceSocketException("Cannot render interface address");
    std::string literal=text;
    // OpenJDK enumerators assign the interface index even when the OS IPv6
    // sockaddr has a zero scope (for example a unique-local tunnel address).
    if(v6&&if_nametoindex(name))literal+="%"+std::string(name);
    return InetSocketAddress(literal,0).getHostAddress();
}
std::string hostname(const sockaddr* address,const char* name){
    char host[NI_MAXHOST];socklen_t size=address->sa_family==AF_INET?sizeof(sockaddr_in):sizeof(sockaddr_in6);
    if(getnameinfo(address,size,host,sizeof(host),nullptr,0,NI_NAMEREQD)==0){
        addrinfo hints{},*raw=nullptr;hints.ai_family=AF_UNSPEC;hints.ai_socktype=SOCK_STREAM;
        if(getaddrinfo(host,nullptr,&hints,&raw)==0){std::unique_ptr<addrinfo,decltype(&freeaddrinfo)> owner(raw,freeaddrinfo);for(auto item=raw;item;item=item->ai_next)if(item->ai_addr&&sameAddress(address,item->ai_addr))return host;}
    }
    return numeric(address,name);
}
using http::Headers;using http::header;using http::discard;using http::part;
}
std::shared_ptr<JavaStringSet> localInterfaceHostnames(){
    ifaddrs* raw=nullptr;if(getifaddrs(&raw)!=0)throw InterfaceSocketException("Cannot enumerate network interfaces");
    std::unique_ptr<ifaddrs,decltype(&freeifaddrs)> owner(raw,freeifaddrs);auto result=std::make_shared<JavaStringSet>();
    for(auto item=raw;item;item=item->ifa_next)if(item->ifa_addr&&(item->ifa_addr->sa_family==AF_INET||item->ifa_addr->sa_family==AF_INET6))result->add(hostname(item->ifa_addr,item->ifa_name));
    return result;
}
std::vector<HostInterface> localHostInterfaces(){
    ifaddrs* raw=nullptr;if(getifaddrs(&raw)!=0)throw InterfaceSocketException("Cannot enumerate network interfaces");
    std::unique_ptr<ifaddrs,decltype(&freeifaddrs)> owner(raw,freeifaddrs);std::vector<HostInterface> result;
    // OpenJDK's BSD enumIPv4Interfaces/addif prepend new groups and addresses.
    // IPv6-only groups do not affect Minima's IPv4 host selection.
    for(auto item=raw;item;item=item->ifa_next)if(item->ifa_addr&&item->ifa_addr->sa_family==AF_INET){
        auto found=std::find_if(result.begin(),result.end(),[&](const auto& entry){return entry.name==item->ifa_name;});
        if(found==result.end()){result.insert(result.begin(),HostInterface{item->ifa_name,bool(item->ifa_flags&IFF_LOOPBACK),bool((item->ifa_flags&IFF_UP)&&(item->ifa_flags&IFF_RUNNING)),{}});found=result.begin();}
        found->addresses.insert(found->addresses.begin(),numeric(item->ifa_addr,item->ifa_name));
    }
    return result;
}
void openURLAndClose(const std::string& input){
    http::initialize();std::string current=input,proxy,proxyTarget;
    for(int redirects=0;redirects<20;++redirects){
        std::unique_ptr<CURL,decltype(&curl_easy_cleanup)> curl(curl_easy_init(),curl_easy_cleanup);if(!curl)throw std::bad_alloc();Headers headers;
        auto set=[&](auto option,auto value){if(curl_easy_setopt(curl.get(),option,value)!=CURLE_OK)throw wire::IOException("Cannot configure URL connection");};
        set(CURLOPT_URL,current.c_str());set(CURLOPT_PROXY,proxy.c_str());set(CURLOPT_NOPROXY,"");set(CURLOPT_NOSIGNAL,1L);set(CURLOPT_HTTP_VERSION,long(CURL_HTTP_VERSION_1_1));set(CURLOPT_USERAGENT,"Java/26.0.1");
        if(!proxy.empty())set(CURLOPT_REQUEST_TARGET,proxyTarget.c_str());
        set(CURLOPT_PROTOCOLS_STR,"http,https");set(CURLOPT_HEADERFUNCTION,header);set(CURLOPT_HEADERDATA,&headers);set(CURLOPT_WRITEFUNCTION,discard);
        auto code=curl_easy_perform(curl.get());if(headers.error)std::rethrow_exception(headers.error);
        // HttpClient.parseHTTP retries a failed GET response once on a fresh socket.
        if(!headers.complete&&(code==CURLE_GOT_NOTHING||code==CURLE_RECV_ERROR)){
            headers=Headers{};set(CURLOPT_FRESH_CONNECT,1L);code=curl_easy_perform(curl.get());if(headers.error)std::rethrow_exception(headers.error);
        }
        if(!headers.complete||(code!=CURLE_OK&&code!=CURLE_WRITE_ERROR))throw wire::IOException(curl_easy_strerror(code));
        if(headers.status>=400)throw wire::IOException("HTTP response error");
        if(headers.status<300||headers.status>307||headers.status==304||headers.status==306||headers.location.empty())return;
        std::unique_ptr<CURLU,decltype(&curl_url_cleanup)> url(curl_url(),curl_url_cleanup);if(!url)throw std::bad_alloc();
        if(curl_url_set(url.get(),CURLUPART_URL,current.c_str(),0)!=CURLUE_OK)throw wire::IOException("Invalid URL");auto protocol=part(url.get(),CURLUPART_SCHEME);
        auto target=part(url.get(),CURLUPART_PATH);char* rawQuery=nullptr;
        if(curl_url_get(url.get(),CURLUPART_QUERY,&rawQuery,0)==CURLUE_OK){std::unique_ptr<char,decltype(&curl_free)> owner(rawQuery,curl_free);target+="?"+std::string(rawQuery);}
        if(curl_url_set(url.get(),CURLUPART_URL,headers.location.c_str(),0)!=CURLUE_OK)throw wire::IOException("Invalid redirect URL");
        if(protocol!=part(url.get(),CURLUPART_SCHEME))return;
        if(headers.status==305){
            // HTTP_USE_PROXY keeps both the original URL and cached requestURI.
            auto host=part(url.get(),CURLUPART_HOST);char* rawPort=nullptr;
            std::string port="80";if(curl_url_get(url.get(),CURLUPART_PORT,&rawPort,0)==CURLUE_OK){std::unique_ptr<char,decltype(&curl_free)> owner(rawPort,curl_free);port=rawPort;}
            proxy="http://"+host+":"+port;proxyTarget=std::move(target);
        }else {current=part(url.get(),CURLUPART_URL);proxy.clear();}
    }
    throw wire::IOException("Too many redirects");
}
}
