#pragma once
#include "ByteIO.hpp"
#include <curl/curl.h>
#include <charconv>
#include <memory>
#include <optional>
#include <string_view>
namespace minima::http {
inline void initialize(){static const bool initialized=[] {if(curl_global_init(CURL_GLOBAL_DEFAULT)!=CURLE_OK)throw wire::IOException("Cannot initialize libcurl");return true;}();(void)initialized;}
using Easy=std::unique_ptr<CURL,decltype(&curl_easy_cleanup)>;
using URL=std::unique_ptr<CURLU,decltype(&curl_url_cleanup)>;
inline Easy easy(){initialize();Easy result(curl_easy_init(),curl_easy_cleanup);if(!result)throw std::bad_alloc();return result;}
inline URL url(){initialize();URL result(curl_url(),curl_url_cleanup);if(!result)throw std::bad_alloc();return result;}
template<class T> void set(CURL* curl,CURLoption option,T value){if(curl_easy_setopt(curl,option,value)!=CURLE_OK)throw wire::IOException("Cannot configure URL connection");}
struct Headers {
    int status=0;bool complete=false;std::string location;std::exception_ptr error;
    bool readSuccessBody=false;std::string body;
};
// Extracted from P2PEnvironment's tested header-only connection. RPC responses
// keep reading only a 200 body; other codes retain the source header-only path.
inline size_t header(char* bytes,size_t size,size_t count,void* receiver){
    auto& state=*static_cast<Headers*>(receiver);size_t length=size*count;
    try{
        std::string_view line(bytes,length);
        if(line.starts_with("HTTP/")){auto space=line.find(' ');state.status=0;state.complete=false;state.location.clear();if(space!=line.npos)std::from_chars(line.data()+space+1,line.data()+line.size(),state.status);}
        else if(line=="\r\n"||line=="\n"){if(state.readSuccessBody&&state.status==101)return 0;if(state.status>=200){state.complete=true;if(!state.readSuccessBody||state.status!=200)return 0;}}
        else if(line.size()>=9){std::string key(line.substr(0,9));for(char& c:key)if(c>='A'&&c<='Z')c+=32;if(key=="location:"){size_t first=9,last=line.size();while(first<last&&(line[first]==' '||line[first]=='\t'))++first;while(last>first&&(line[last-1]=='\r'||line[last-1]=='\n'||line[last-1]==' '||line[last-1]=='\t'))--last;state.location=line.substr(first,last-first);}}
    }catch(...){state.error=std::current_exception();return 0;}return length;
}
inline size_t body(char* bytes,size_t size,size_t count,void* receiver){auto& state=*static_cast<Headers*>(receiver);size_t length=size*count;try{state.body.append(bytes,length);}catch(...){state.error=std::current_exception();return 0;}return length;}
inline size_t discard(char*,size_t,size_t,void*){return 0;}
inline std::optional<std::string> optionalPart(CURLU* url,CURLUPart what,unsigned flags=0){char* raw=nullptr;if(curl_url_get(url,what,&raw,flags)!=CURLUE_OK)return {};std::unique_ptr<char,decltype(&curl_free)> owner(raw,curl_free);return std::string(raw);}
inline std::string part(CURLU* url,CURLUPart what,unsigned flags=0){auto result=optionalPart(url,what,flags);if(!result)throw wire::IOException("Invalid URL part");return *result;}
}
