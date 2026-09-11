#include "InetSocketAddress.hpp"
#include "JavaExceptions.hpp"
#include "JavaLowerCase.hpp"
#include "JavaUnicode.hpp"
#include "JavaStringIgnoreCase.hpp"
#include <algorithm>
#include <bit>
#include <charconv>
#include <stdexcept>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <unicode/uchar.h>
namespace minima {
namespace {
void portCheck(int32_t port) {if(port<0||port>65535)throw std::invalid_argument("port out of range:"+std::to_string(port));}
std::vector<uint8_t> ipv4(const std::string& text) {
    if(text.empty()||text.size()>15)return {};
    std::vector<uint64_t> parts;uint64_t part=0;bool digit=false;
    for(char c:text) {
        if(c=='.') {if(!digit||part>255||parts.size()==3)return {};parts.push_back(part);part=0;digit=false;}
        else if(c>='0'&&c<='9'){part=part*10+unsigned(c-'0');digit=true;}
        else return {};
    }
    if(!digit||part>=(uint64_t(1)<<(8*(4-parts.size()))))return {};
    std::vector<uint8_t> bytes;for(auto p:parts)bytes.push_back(uint8_t(p));
    for(size_t i=parts.size();i<4;++i)bytes.push_back(uint8_t(part>>(8*(3-i))));
    return bytes;
}
bool mapped(const std::vector<uint8_t>& address) {return address.size()==16&&std::all_of(address.begin(),address.begin()+10,[](uint8_t b){return b==0;})&&address[10]==255&&address[11]==255;}
bool bsdLiteral(const std::string& text) {
    if(text.empty()||text.front()<'0'||text.front()>'9'||text.back()=='.')return false;
    size_t start=0,field=0;
    while(start<text.size()) {
        auto end=text.find('.',start);bool last=end==std::string::npos;if(last)end=text.size();
        if(end==start||field>3)return false;
        int radix=10;size_t pos=start;
        if(end-start>1&&text[start]=='0'){if(text[start+1]=='x'||text[start+1]=='X'){radix=16;pos+=2;}else{radix=8;++pos;}}
        if(pos==end)return false;
        uint64_t value=0;
        for(;pos<end;++pos){char c=text[pos];int digit=c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;if(digit<0||digit>=radix)return false;value=value*unsigned(radix)+unsigned(digit);}
        uint64_t limit=last?((uint64_t(1)<<((4-field)*8))-1):255;
        if(value>limit)return false;
        if(last)return true;
        ++field;start=end+1;
    }
    return false;
}
bool validScope(const std::string& name,const std::vector<uint8_t>& address) {
    if(!if_nametoindex(name.c_str()))return false;
    ifaddrs* raw=nullptr;if(getifaddrs(&raw)!=0)return false;
    std::unique_ptr<ifaddrs,decltype(&freeifaddrs)> list(raw,freeifaddrs);
    const bool link=address[0]==0xfe&&(address[1]&0xc0)==0x80,site=address[0]==0xfe&&(address[1]&0xc0)==0xc0;
    for(auto current=raw;current;current=current->ifa_next)if(current->ifa_addr&&name==current->ifa_name&&current->ifa_addr->sa_family==AF_INET6) {
        auto& local=reinterpret_cast<sockaddr_in6*>(current->ifa_addr)->sin6_addr;
        if((!link||IN6_IS_ADDR_LINKLOCAL(&local))&&(!site||IN6_IS_ADDR_SITELOCAL(&local)))return true;
    }
    return false;
}
}
InetSocketAddress::InetSocketAddress(const std::optional<std::string>& host,int32_t port) {
    if(!host)throw std::invalid_argument("hostname can't be null");resolve(*host);portCheck(port);mPort=port;
}
InetSocketAddress::InetSocketAddress(int32_t port) {portCheck(port);mPort=port;mHostname="0.0.0.0";mAddress={0,0,0,0};}
SocketAddressPtr InetSocketAddress::fromResolvedHost(const std::string& host,int32_t port) {
    auto result=SocketAddressPtr(new InetSocketAddress);result->resolve(host);
    if(result->isUnresolved())throw JavaTypedException("java.net.UnknownHostException",host);
    portCheck(port);result->mPort=port;return result;
}
SocketAddressPtr InetSocketAddress::createUnresolved(const std::optional<std::string>& host,int32_t port) {
    portCheck(port);if(!host)throw std::invalid_argument("hostname can't be null");
    auto result=SocketAddressPtr(new InetSocketAddress);result->mHostname=*host;result->mPort=port;return result;
}
void InetSocketAddress::resolve(const std::string& host) {
    mHostname=host;
    if(host.empty()){mHostname="localhost";mAddress={127,0,0,1};return;}
    if(host.find('\0')!=std::string::npos)return;
    std::string literal=host;bool bracket=literal.starts_with('[');
    if(bracket){if(literal.size()<3||!literal.ends_with(']'))return;literal=literal.substr(1,literal.size()-2);}
    if(!bracket) {mAddress=ipv4(literal);if(!mAddress.empty()){mHostname.reset();return;}}
    if(bracket||literal.find(':')!=std::string::npos) {
        auto percent=literal.find('%');std::string scope;
        if(percent!=std::string::npos){scope=literal.substr(percent+1);literal.resize(percent);if(scope.empty())return;}
        in6_addr raw{};if(inet_pton(AF_INET6,literal.c_str(),&raw)!=1)return;
        std::vector<uint8_t> address(raw.s6_addr,raw.s6_addr+16);
        if(mapped(address)){if(!scope.empty())return;address=std::vector<uint8_t>(address.begin()+12,address.end());}
        else if(!scope.empty()) {
            uint32_t index=0;auto parsed=std::from_chars(scope.data(),scope.data()+scope.size(),index);
            if(parsed.ec==std::errc{}&&parsed.ptr==scope.data()+scope.size()&&index<=INT32_MAX)scope=std::to_string(index);
            else if(!validScope(scope,address))return;
        }
        mAddress=std::move(address);mScope=std::move(scope);mHostname.reset();return;
    }
    // A numeric spelling rejected by Java must not become BSD octal/hex via libc.
    if(bsdLiteral(literal))return;
    addrinfo hints{},*raw=nullptr;hints.ai_family=AF_UNSPEC;hints.ai_socktype=SOCK_STREAM;hints.ai_flags=AI_ADDRCONFIG;
    if(getaddrinfo(host.c_str(),nullptr,&hints,&raw)!=0) {
        if(javaLowerCase(host)=="localhost"){mHostname="localhost";mAddress={127,0,0,1};}
        return;
    }
    std::unique_ptr<addrinfo,decltype(&freeaddrinfo)> list(raw,freeaddrinfo);
    // Match the JDK's default IPv4-first lookup policy.
    addrinfo* selected=nullptr;
    for(auto current=raw;current;current=current->ai_next)if(current->ai_family==AF_INET||current->ai_family==AF_INET6){if(!selected)selected=current;if(current->ai_family==AF_INET){selected=current;break;}}
    if(!selected)return;
    if(selected->ai_family==AF_INET){auto bytes=reinterpret_cast<const uint8_t*>(&reinterpret_cast<sockaddr_in*>(selected->ai_addr)->sin_addr);mAddress.assign(bytes,bytes+4);}
    else {auto addr=reinterpret_cast<sockaddr_in6*>(selected->ai_addr);mAddress.assign(addr->sin6_addr.s6_addr,addr->sin6_addr.s6_addr+16);if(addr->sin6_scope_id)mScope=std::to_string(addr->sin6_scope_id);}
}
std::string InetSocketAddress::getHostAddress() const {
    if(isUnresolved())throw std::runtime_error("Null resolved address");std::string result;
    if(mAddress.size()==4){for(uint8_t byte:mAddress){if(!result.empty())result+='.';result+=std::to_string(byte);}}
    else {for(size_t i=0;i<16;i+=2){if(i)result+=':';char buffer[5];auto end=std::to_chars(buffer,buffer+5,(unsigned(mAddress[i])<<8)|mAddress[i+1],16).ptr;result.append(buffer,end);}if(!mScope.empty())result+='%'+mScope;}
    return result;
}
std::string InetSocketAddress::getHostString() const {return mHostname?*mHostname:getHostAddress();}
std::string InetSocketAddress::toString() const {return mHostname.value_or("")+"/"+(isUnresolved()?"<unresolved>":mAddress.size()==16?"["+getHostAddress()+"]":getHostAddress())+":"+std::to_string(mPort);}
bool InetSocketAddress::equals(const InetSocketAddress& other) const {
    return equalityKey()==other.equalityKey();
}
std::string InetSocketAddress::equalityKey() const {
    std::string result=std::to_string(mPort)+":";
    if(!isUnresolved()) {result+='R';for(auto byte:mAddress){result+=std::to_string(byte);result+=',';}return result;}
    return result+"U"+javaStringIgnoreCaseKey(mHostname.value());
}
int32_t InetSocketAddress::hashCode() const {
    uint32_t hash=0;
    if(isUnresolved())for(char16_t unit:unicode::decodeUTF8(javaLowerCase(mHostname.value())))hash=31u*hash+uint32_t(unit);
    else if(mAddress.size()==4)for(uint8_t byte:mAddress)hash=(hash<<8)|byte;
    else for(size_t i=0;i<16;i+=4){uint32_t component=0;for(size_t j=i;j<i+4;++j)component=(component<<8)+uint32_t(int32_t(std::bit_cast<int8_t>(mAddress[j])));hash+=component;}
    return std::bit_cast<int32_t>(hash+uint32_t(mPort));
}
}
