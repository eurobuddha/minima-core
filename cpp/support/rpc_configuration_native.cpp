#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/network/rpc/Authorizer.hpp"
#include "support/Base64.hpp"
#include <atomic>
#include <iostream>
#include <latch>
#include <thread>
#include <vector>
using namespace minima;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
static std::string header(const std::string& password){
    auto credentials="minima:"+password;
    return "Basic "+encodeBase64({reinterpret_cast<const uint8_t*>(credentials.data()),credentials.size()});
}
int main()try{
    // The same simultaneous-write/snapshot check as notify_http_worker, extended
    // to nullable passwords and the actual authorization reader.
    std::string first(8192,'a'),second(8192,'b');
    GeneralParams::RPC_PASSWORD=first;auto retained=GeneralParams::RPC_PASSWORD.load();
    UserDB db;db.getRPCUsers(); // Initialize before concurrent read-only access.
    GeneralParams::RPC_AUTHENTICATE=true;
    std::atomic<bool> intact{true};std::latch start(1);std::vector<std::jthread> workers;
    for(int thread=0;thread<4;++thread)workers.emplace_back([&,thread]{
        start.wait();
        for(int i=0;i<2000;++i){
            if(thread<2){
                GeneralParams::RPC_PASSWORD=i%3==0?std::nullopt:std::optional(i%3==1?first:second);
                GeneralParams::RPC_ENABLED=i%2==0;
            }else{
                auto value=GeneralParams::RPC_PASSWORD.load();
                if(value&&*value!=first&&*value!=second)intact=false;
                (void)GeneralParams::RPC_ENABLED.load();
                if(Authorizer::checkAuchCredentials(header("never-assigned"),db).getBoolean("valid"))intact=false;
            }
        }
    });
    start.count_down();for(auto& worker:workers)worker.join();
    require(intact,"Torn password snapshot or invalid credential accepted");
    require(retained&&*retained==first,"Retained password snapshot changed");
    GeneralParams::RPC_PASSWORD=std::nullopt;
    require(!Authorizer::checkAuchCredentials(header("null"),db).getBoolean("valid"),"Null reference accepted literal null");
    require(!Authorizer::checkAuchCredentials(header(""),db).getBoolean("valid"),"Null reference accepted empty password");
    GeneralParams::RPC_PASSWORD=std::string();
    require(Authorizer::checkAuchCredentials(header(""),db).getBoolean("valid"),"Empty configured password rejected");
    GeneralParams::RPC_AUTHENTICATE=false;
    require(Authorizer::checkAuchCredentials(header("anything"),db).getBoolean("valid"),"Authentication disabled differs");
    GeneralParams::resetDefaults();
    std::cout<<"RPC configuration: 8000 concurrent operations, retained snapshot and null/empty authorization passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
