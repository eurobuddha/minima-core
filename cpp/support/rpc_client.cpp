#include "org/minima/utils/RPCClient.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "support/CurlTransport.hpp"
#include <sstream>
#include <iostream>
using namespace minima;
using namespace minima::json;
int main(){std::string line;while(std::getline(std::cin,line))try{
    auto input=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);auto string=[&](const auto& key)->RPCClient::String{const auto& value=input->get(key);return value.isNull()?std::nullopt:std::optional(value.asString());};
    RPCClient::USER_AGENT=input->containsKey("agent")?string("agent"):RPCClient::String("Minima/1.0");auto url=string("url"),user=string("user"),password=string("password"),params=string("data"),type=string("type");std::shared_ptr<RPCClient::SSLContext> context;
    if(auto ca=string("ca"))context=std::make_shared<RPCClient::SSLContext>([path=*ca](CURL* curl){http::set(curl,CURLOPT_CAINFO,path.c_str());});
    JSONObject output;std::ostringstream printed;auto prior=std::cout.rdbuf(printed.rdbuf());
    try{std::string result;auto method=input->getString("method");
        if(method=="get")result=RPCClient::sendGET(url);else if(method=="basic")result=RPCClient::sendGETBasicAuth(url,user,password);else if(method=="getHTTPS")result=RPCClient::sendGETHTTPS(url);else if(method=="getSSL")result=RPCClient::sendGETSSL(url);else if(method=="basicSSL")result=RPCClient::sendGETBasicAuthSSL(url,user,password,context);else if(method=="put")result=RPCClient::sendPUT(url);else if(method=="post")result=RPCClient::sendPOST(url,params);else if(method=="postType")result=RPCClient::sendPOST(url,params,type);else if(method=="postHTTPS")result=RPCClient::sendPOSTHTTPS(url,params,type);else if(method=="auth")result=RPCClient::sendGETAuth(url,string("token"));else throw std::invalid_argument(method);output.put("result",result);
    }catch(const wire::IOException& e){output.put("exception",javaIOExceptionText(e));}catch(const std::exception& e){output.put("exception",javaExceptionText(e));}
    std::cout.rdbuf(prior);output.put("stdout",printed.str());std::cout<<output.toJSONString()<<'\n';
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';std::cout<<"{\"fatal\":true}\n";}}
