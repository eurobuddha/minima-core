#include "org/minima/system/commands/CommandRunner.hpp"
#include "org/minima/system/commands/base/hash.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/JavaExceptions.hpp"
#include <array>
#include <barrier>
#include <thread>
#include <iostream>
using namespace minima;
int main(){
    GeneralParams::RPC_PORT=19005;
    using Registry=std::vector<std::shared_ptr<Command>>;
    std::array<Registry*,8> arrays{};std::array<std::thread,8> threads;std::barrier gate(8);
    for(size_t i=0;i<threads.size();++i)threads[i]=std::thread([&,i]{gate.arrive_and_wait();arrays[i]=&CommandRunner::ALL_COMMANDS();});
    for(auto& thread:threads)thread.join();
    auto& registry=CommandRunner::ALL_COMMANDS();json::JSONObject out;bool same=true;for(auto* array:arrays)same&=array==&registry;out.put("sameRegistry",same);
    json::JSONArray rows;
    for(auto& prototype:registry){
        json::JSONObject row;row.put("name",*prototype->getName());row.put("help",*prototype->getHelp());
        auto full=prototype->getFullHelp();row.put("fullhelp",full?json::JSONValue(*full):json::JSONValue());
        json::JSONArray params;for(auto& p:prototype->getValidParams())params.add(p);row.put("params",params);
        auto a=CommandRunner::getRunner().getCommandOnly(*prototype->getName()),b=CommandRunner::getRunner().getCommandOnly(*prototype->getName());
        row.put("fresh",a!=b&&a!=prototype&&b!=prototype);row.put("freshparams",*a->getParams());rows.add(row);
    }
    out.put("commands",rows);
    auto original=registry[0];registry[0]=std::make_shared<commands::hash>();out.put("mutationVisible",CommandRunner::getRunner().getCommandOnly("quit")==nullptr);
    registry[0]=original;out.put("restored",CommandRunner::getRunner().getCommandOnly("quit")!=nullptr);
    json::JSONArray errors;
    for(auto input:{"jnitest maxattempts:0","benchmark hashes:0","jniminingtest maxattempts:0"}){
        try{CommandRunner::getRunner().runMultiCommand(input);errors.add("unexpected success");}catch(const JavaError& e){errors.add(e.toString());}
    }
    out.put("jniMissingLibrary",errors);std::cout<<out.toJSONString()<<'\n';
}
