#include "org/minima/database/userprefs/UserDB.hpp"
#include "org/minima/system/network/rpc/Authorizer.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/Base64.hpp"
#include <atomic>
#include <iostream>
#include <latch>
#include <thread>
using namespace minima;
using namespace minima::json;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
static JSONValue shared(const std::shared_ptr<JSONObject>& object){JSONValue result;result.value=object;return result;}
static std::string credentials(const std::string& text){return "Basic "+encodeBase64({reinterpret_cast<const uint8_t*>(text.data()),text.size()});}
int main()try{
    auto user=std::make_shared<JSONObject>();user->put("username","alice");user->put("password","synthetic");user->put("mode","read");
    UserDB db;auto held=db.getRPCUsers();held->add(shared(user));auto rootHeld=db.getAllData();
    GeneralParams::RPC_AUTHENTICATE=true;GeneralParams::RPC_PASSWORD="synthetic";
    auto replacementWire=MiniString("{\"rpcusers_allusers\":[{\"username\":\"alice\",\"password\":\"synthetic\",\"mode\":\"read\"}],\"root\":true}").serialise();
    const auto admin=credentials("minima:synthetic"),alice=credentials("alice:synthetic");
    std::atomic<bool> okay{true};std::atomic<int> serializations{0},concurrentChanges{0};
    std::mutex errorLock;std::string errorText;std::latch start(1);std::vector<std::jthread> workers;
    auto serialize=[&](const auto& action){
        try{auto text=action();(void)JSONValue::parseWithException(text);++serializations;}
        catch(const JavaTypedException& e){if(e.toString()!="java.util.ConcurrentModificationException")throw;++concurrentChanges;}
    };
    for(int index=0;index<4;++index)workers.emplace_back([&,index]{
        start.wait();try{
            for(int i=0;i<400;++i){
                if(index==0)held->add(shared(user));
                else if(index==1){
                    auto next=std::make_shared<JSONArray>();next->add(shared(user));db.setRPCUsers(next);
                    size_t offset=0;db.readDataStream(replacementWire,offset);require(offset==replacementWire.size(),"Root replacement wire not consumed");
                }else{
                    require(Authorizer::checkAuchCredentials(admin,db).getBoolean("valid"),"Valid main credentials rejected");
                    auto auth=Authorizer::checkAuchCredentials(alice,db);
                    if(auth.getBoolean("valid"))require(auth.getString("mode")=="read","Corrupted user mode");
                    serialize([&]{return held->toJSONString();});
                    serialize([&]{auto bytes=db.serialise();size_t offset=0;return MiniString::deserialise(bytes,offset).toString();});
                }
            }
        }catch(const std::exception& e){okay=false;std::lock_guard guard(errorLock);errorText=e.what();}
    });
    start.count_down();for(auto& worker:workers)worker.join();
    require(okay,errorText.c_str());require(serializations>0,"No serialization completed");
    require(held->size()==401,"Lost append to retained array");
    require(held!=db.getRPCUsers()&&rootHeld!=db.getAllData(),"Root or array replacement lost identity");
    require(std::get<std::shared_ptr<JSONObject>>(held->get(0).value)==user,"Nested record identity changed");
    JSONObject owner;auto child=std::make_shared<JSONObject>();std::weak_ptr<JSONObject> weak=child;
    owner.put("child",shared(child));auto retained=owner.get("child");owner.put("child",nullptr);child.reset();
    require(!weak.expired(),"Copied value lost its nested owner");retained=JSONValue();require(weak.expired(),"Removed child retained unexpectedly");
    GeneralParams::resetDefaults();
    std::cout<<"Shared JSON: 1600 concurrent iterations, "<<serializations<<" valid serializations, "<<concurrentChanges<<" permitted iterator failures; retained array/root/child ownership passed\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
