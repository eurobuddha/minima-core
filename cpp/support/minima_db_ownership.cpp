#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include <barrier>
#include <iostream>
#include <thread>
using namespace minima;
namespace {
MinimaDBServices services(){return {
    {[](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{throw std::logic_error("Unexpected SQL access in owner fixture");},[]{return false;},[](const auto&,bool){}},
    []{return false;},[](auto){},[](auto){},[](const auto&,const auto&){},[](bool,const auto&){},[](int){throw std::logic_error("Unexpected halt");},[]{return "ownership-fixture";}};}
std::shared_ptr<MegaMMR> populated(int amount){auto mega=std::make_shared<MegaMMR>();mega->getMMR().addEntry(MMRData(MiniData::fromHex("0x01"),MiniNumber(amount)));return mega;}
}
int main(){
    GeneralParams::IS_MEGAMMR=true;MinimaDB::createDB(services());auto db=MinimaDB::getDB();
    auto mega=populated(7);db->hardSetMegaMMR(mega);auto held=MMR::megaMMRLookup();std::weak_ptr<MMR> old=held;
    mega->clear();if(held->getEntry(0,MMREntryNumber(0))->getMMRData()->getValue().getAsInt()!=7||mega->getMMR().getTotalEntries()!=0)return 1;
    held.reset();if(!old.expired())return 1;
    // The callback transfers its last owner into getEntry. The returned entry
    // stays usable after getEntry releases that owner at the end of the walk.
    auto transferred=populated(9)->getSharedMMR();std::weak_ptr<MMR> transferredWeak=transferred;
    MMR::megaMMRLookup=[&]{return std::exchange(transferred,{});};MMR query;query.clearParent();
    auto entry=query.getEntry(0,MMREntryNumber(0));
    if(!transferredWeak.expired()||entry->getMMRData()->getValue().getAsInt()!=9)return 1;
    MinimaDB::bindRuntime();db->hardSetMegaMMR(populated(1));
    std::atomic<int> failures=0;std::barrier start(8);std::vector<std::thread> workers;
    for(int i=0;i<8;++i)workers.emplace_back([&,i]{start.arrive_and_wait();
        for(int n=0;n<1000;++n){if(i==0)db->hardSetMegaMMR(populated(n%2+1));else{auto value=query.getEntry(0,MMREntryNumber(0));if(value->isEmpty())++failures;else{int amount=value->getMMRData()->getValue().getAsInt();if(amount!=1&&amount!=2)++failures;}}}
    });for(auto& worker:workers)worker.join();
    std::cout<<"MinimaDB ownership: retained reset MMR, transferred last owner, 7000 lookups during 1000 replacements: "<<(failures==0?"PASS":"FAIL")<<'\n';return failures==0?0:1;
}
