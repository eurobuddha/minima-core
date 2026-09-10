#include "RPCClient.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include "support/CurlTransport.hpp"
#include "support/Base64.hpp"
#include "support/JavaExceptions.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
namespace minima {
namespace {
using String=RPCClient::String;
const std::string& required(const String& value){if(!value)throw JavaNullPointerException("null RPC string");return *value;}
std::string trim(const std::string& input){size_t first=0,last=input.size();while(first<last&&uint8_t(input[first])<=32)++first;while(last>first&&uint8_t(input[last-1])<=32)--last;return input.substr(first,last-first);}
bool emptyQuery(const std::string& input){auto text=trim(input);auto fragment=text.find('#');if(fragment!=text.npos)text.resize(fragment);return text.ends_with('?');}
std::string rawTarget(const std::string& input){auto text=trim(input);auto start=text.find("://");if(start==text.npos)return "/";start=text.find_first_of("/?#",start+3);if(start==text.npos||text[start]=='#')return "/";auto end=text.find('#',start);auto result=text.substr(start,end==text.npos?end:end-start);if(result.starts_with('?'))result.insert(0,"/");return result;}
void checkHeader(const String& value){if(!value)return;for(size_t i=0;i<value->size();++i)if((*value)[i]=='\n'&&(i+1==value->size()||((*value)[i+1]!=' '&&(*value)[i+1]!='\t')))throw JavaIllegalArgumentException("Illegal character(s) in message header value: "+*value);}
http::URL parse(const String& host,bool https){
    if(!host)throw wire::TypedIOException("java.net.MalformedURLException","null URL");
    auto text=trim(*host);if(text.size()>=4&&std::equal(text.begin(),text.begin()+4,"url:",[](char a,char b){return std::tolower(static_cast<unsigned char>(a))==b;}))text.erase(0,4);
    if(text.find('\n')!=text.npos)throw wire::TypedIOException("java.net.MalformedURLException","Illegal character in URL");
    // Keep C string URL APIs from truncating NUL or rejecting a CR path before
    // Java's connection phase. The unmodified request target is retained below.
    for(size_t i=0;i<text.size();++i)if(text[i]=='\0'||text[i]=='\r'){auto escape=text[i]=='\0'?"%00":"%0D";text.replace(i,1,escape);i+=2;}
    auto result=http::url();auto code=curl_url_set(result.get(),CURLUPART_URL,text.c_str(),CURLU_NON_SUPPORT_SCHEME|CURLU_PATH_AS_IS|CURLU_ALLOW_SPACE);
    if(code!=CURLUE_OK)throw wire::TypedIOException("java.net.MalformedURLException",curl_url_strerror(code));
    auto scheme=http::part(result.get(),CURLUPART_SCHEME);
    if(scheme!="http"&&scheme!="https"){
        if(scheme=="file"||scheme=="ftp"||scheme=="jar")throw JavaClassCastException("URL connection is not HTTP");
        throw wire::TypedIOException("java.net.MalformedURLException","unknown protocol: "+scheme);
    }
    if(https&&scheme!="https")throw JavaClassCastException("URL connection is not HTTPS");return result;
}
std::string target(CURLU* url){auto result=http::part(url,CURLUPART_PATH);if(auto query=http::optionalPart(url,CURLUPART_QUERY))result+="?"+*query;return result;}
bool sameDestination(CURLU* first,CURLU* second){auto a=http::part(first,CURLUPART_HOST),b=http::part(second,CURLUPART_HOST);for(char& c:a)c=char(std::tolower(static_cast<unsigned char>(c)));for(char& c:b)c=char(std::tolower(static_cast<unsigned char>(c)));return a==b&&http::part(first,CURLUPART_PORT,CURLU_DEFAULT_PORT)==http::part(second,CURLUPART_PORT,CURLU_DEFAULT_PORT);}
struct Request {
    http::URL url;
    String original;
    std::string method;
    String agent=RPCClient::USER_AGENT,auth,type,params;
    String originalParams;
    String initialTarget;
    std::shared_ptr<RPCClient::SSLContext> tls;
    bool keepAlive=false,hasEmptyQuery=false;
    Request(const String& host,std::string verb,bool https):url(parse(host,https)),original(host),method(std::move(verb)),initialTarget(rawTarget(*host)),hasEmptyQuery(emptyQuery(*host)){checkHeader(agent);}
};
std::string basic(const String& user,const String& password){auto bytes=user.value_or("null")+":"+required(password);return "Basic "+encodeBase64({reinterpret_cast<const uint8_t*>(bytes.data()),bytes.size()});}
[[noreturn]] void transportError(CURLcode code){
    if(code==CURLE_PARTIAL_FILE)throw wire::IOException("Premature EOF");
    if(code==CURLE_GOT_NOTHING)throw wire::TypedIOException("java.net.SocketException","Unexpected end of file from server");
    const char* kind=nullptr;
    switch(code){
        case CURLE_COULDNT_CONNECT:kind="java.net.ConnectException";break;
        case CURLE_COULDNT_RESOLVE_HOST:kind="java.net.UnknownHostException";break;
        case CURLE_OPERATION_TIMEDOUT:kind="java.net.SocketTimeoutException";break;
        case CURLE_SEND_ERROR:case CURLE_RECV_ERROR:kind="java.net.SocketException";break;
        case CURLE_PEER_FAILED_VERIFICATION:case CURLE_SSL_CONNECT_ERROR:kind="javax.net.ssl.SSLHandshakeException";break;
        default:throw wire::IOException(curl_easy_strerror(code));
    }
    throw wire::TypedIOException(kind,curl_easy_strerror(code));
}
std::string perform(Request request){
    if(trim(*request.original).find('\0')!=std::string::npos)throw wire::IOException("Failed to select a proxy");
    std::string proxy,proxyTarget;
    for(int redirects=0;redirects<20;++redirects){
        auto curl=http::easy();auto set=[&](auto option,auto value){http::set(curl.get(),option,value);};http::Headers response;response.readSuccessBody=true;
        auto current=http::part(request.url.get(),CURLUPART_URL);set(CURLOPT_CURLU,request.url.get());set(CURLOPT_PROXY,proxy.c_str());set(CURLOPT_NOPROXY,"");set(CURLOPT_NOSIGNAL,1L);set(CURLOPT_CONNECTTIMEOUT_MS,10000L);set(CURLOPT_HTTP_VERSION,long(CURL_HTTP_VERSION_1_1));set(CURLOPT_PATH_AS_IS,1L);
        set(CURLOPT_PROTOCOLS_STR,"http,https");set(CURLOPT_HEADERFUNCTION,http::header);set(CURLOPT_HEADERDATA,&response);set(CURLOPT_WRITEFUNCTION,http::body);set(CURLOPT_WRITEDATA,&response);set(CURLOPT_HTTP_CONTENT_DECODING,0L);
        // URL user-info does not authorize Java HttpURLConnection requests.
        // Explicit Minima Authorization headers are added below.
        set(CURLOPT_HTTPAUTH,long(CURLAUTH_NONE));
        auto requestTarget=request.initialTarget.value_or(target(request.url.get()));if(!request.initialTarget&&request.hasEmptyQuery&&!http::optionalPart(request.url.get(),CURLUPART_QUERY))requestTarget+='?';
        set(CURLOPT_REQUEST_TARGET,proxy.empty()?requestTarget.c_str():proxyTarget.c_str());
        std::unique_ptr<curl_slist,decltype(&curl_slist_free_all)> headers(nullptr,curl_slist_free_all);
        auto add=[&](const std::string& text){auto raw=curl_slist_append(headers.get(),text.c_str());if(!raw)throw std::bad_alloc();headers.release();headers.reset(raw);};
        add("Proxy-Connection:");
        auto agent=request.agent.value_or("Java/26.0.1");add(agent.empty()?"User-Agent;":"User-Agent: "+agent);add(request.keepAlive?"Connection: keep-alive":"Connection: close");
        if(request.auth)add("Authorization: "+*request.auth);
        if(request.method=="POST"){
            add(request.type?request.type->empty()?"Content-Type;":"Content-Type: "+*request.type:"Content-Type: application/x-www-form-urlencoded");add("Expect:");
            const auto& body=required(request.params);set(CURLOPT_POSTFIELDS,body.c_str());set(CURLOPT_POSTFIELDSIZE_LARGE,curl_off_t(body.size()));
        }else if(request.method=="PUT")set(CURLOPT_CUSTOMREQUEST,"PUT");
        set(CURLOPT_HTTPHEADER,headers.get());if(request.tls)request.tls->configure(curl.get());
        auto code=curl_easy_perform(curl.get());if(response.error)std::rethrow_exception(response.error);
        if(!response.complete&&(code==CURLE_GOT_NOTHING||code==CURLE_RECV_ERROR||response.status==101)){
            response=http::Headers{};response.readSuccessBody=true;set(CURLOPT_FRESH_CONNECT,1L);code=curl_easy_perform(curl.get());if(response.error)std::rethrow_exception(response.error);
        }
        if(response.status==101)throw wire::TypedIOException("java.net.ProtocolException","Unexpected 101 response from server");
        if(!response.complete||(code!=CURLE_OK&&!(code==CURLE_WRITE_ERROR&&response.status!=200)))transportError(code);
        int status=response.status;
        if(status>=300&&status<=307&&status!=304&&status!=306&&!response.location.empty()){
            auto next=http::url();if(curl_url_set(next.get(),CURLUPART_URL,current.c_str(),CURLU_PATH_AS_IS|CURLU_ALLOW_SPACE)!=CURLUE_OK||curl_url_set(next.get(),CURLUPART_URL,response.location.c_str(),CURLU_ALLOW_SPACE)!=CURLUE_OK)throw wire::TypedIOException("java.net.MalformedURLException","Invalid redirect URL");
            if(http::part(request.url.get(),CURLUPART_SCHEME)==http::part(next.get(),CURLUPART_SCHEME)){
                if(status==305){proxy="http://"+http::part(next.get(),CURLUPART_HOST)+":"+http::optionalPart(next.get(),CURLUPART_PORT).value_or("80");proxyTarget=requestTarget;}
                else {
                    if(request.method=="POST"&&status!=307){request.method="GET";request.params.reset();request.type.reset();request.auth.reset();request.agent="Java/26.0.1";request.keepAlive=true;}
                    else if(!sameDestination(request.url.get(),next.get()))request.auth.reset();
                    request.initialTarget.reset();request.hasEmptyQuery=emptyQuery(response.location);request.url=std::move(next);proxy.clear();
                }
                continue;
            }
        }
        if(status==200){auto result=MiniString(response.body).toString();std::erase(result,'\r');std::erase(result,'\n');return result;}
        // The public method still reports POST after a redirect changed the
        // underlying request method to GET.
        if(request.originalParams.has_value())throw wire::IOException("POST request not HTTP_OK resp:"+std::to_string(status)+" @ "+request.original.value()+" params "+*request.originalParams);
        std::cout<<request.method<<" request not HTTP_OK resp:"<<status<<" @ "<<request.original.value()<<'\n';return {};
    }
    throw wire::TypedIOException("java.net.ProtocolException","Server redirected too many times (20)");
}
std::string post(const String& host,const String& params,const String& type,bool https){
    Request request(host,"POST",https);request.type=type;checkHeader(type);
    if(!params){
        if(trim(*host).find('\0')!=std::string::npos)throw wire::IOException("Failed to select a proxy");
        // Buffered HttpURLConnection.getOutputStream connects before the Java
        // caller dereferences zParams to encode its bytes.
        auto curl=http::easy();http::set(curl.get(),CURLOPT_CURLU,request.url.get());http::set(curl.get(),CURLOPT_PROXY,"");http::set(curl.get(),CURLOPT_NOSIGNAL,1L);http::set(curl.get(),CURLOPT_CONNECTTIMEOUT_MS,10000L);http::set(curl.get(),CURLOPT_CONNECT_ONLY,1L);auto code=curl_easy_perform(curl.get());if(code!=CURLE_OK)transportError(code);throw JavaNullPointerException("null POST parameters");
    }
    request.params=params;request.originalParams=params;return perform(std::move(request));
}
}
std::string RPCClient::sendGET(const String& host){return required(host).starts_with("https")?sendGETHTTPS(host):sendGETBasicAuth(host,"","");}
std::string RPCClient::sendGETBasicAuth(const String& host,const String& user,const String& password){Request request(host,"GET",false);if(!required(password).empty())request.auth=basic(user,password);return perform(std::move(request));}
std::string RPCClient::sendGETHTTPS(const String& host){return perform(Request(host,"GET",true));}
std::string RPCClient::sendGETSSL(const String& host){return sendGETBasicAuthSSL(host,"","",nullptr);}
std::string RPCClient::sendGETBasicAuthSSL(const String& host,const String& user,const String& password,const std::shared_ptr<SSLContext>& context){Request request(host,"GET",true);if(!context)throw JavaNullPointerException("null SSLContext");request.tls=context;if(!required(password).empty())request.auth=basic(user,password);return perform(std::move(request));}
std::string RPCClient::sendPUT(const String& host){return perform(Request(host,"PUT",false));}
std::string RPCClient::sendPOST(const String& host,const String& params){return required(host).starts_with("https")?sendPOSTHTTPS(host,params,std::nullopt):sendPOST(host,params,std::nullopt);}
std::string RPCClient::sendPOST(const String& host,const String& params,const String& type){return post(host,params,type,false);}
std::string RPCClient::sendPOSTHTTPS(const String& host,const String& params,const String& type){return post(host,params,type,true);}
std::string RPCClient::sendGETAuth(const String& host,const String& token){Request request(host,"GET",false);request.auth="Basic "+token.value_or("null");checkHeader(request.auth);return perform(std::move(request));}
}
