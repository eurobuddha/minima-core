#include "test.hpp"
#include "org/minima/database/MinimaDB.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include <iostream>
namespace minima::commands {
test::test():Command("test","test Funxtion"){}
json::JSONValue test::runCommand(){
    auto result=getJSONReply();
    MinimaLogger::log("About to close and reopen DBs");
    sqlRequired(MinimaDB::getDB()).refreshSQLDB();
    MinimaLogger::log("DBs reopened..");
    return result;
}
void test::main(){
    std::cout<<"Start test..\n";
    for(int i=0;i<100000;++i){
        auto data=MiniData::getRandomData(32);auto address=Address::makeMinimaAddress(data);auto length=address.size();
        if(length!=63)std::cout<<length<<' '<<address<<' '<<data.to0xString()<<'\n';
    }
    std::cout<<"Finish test..\n";
}
// The unused private Java getFileFromResourceAsStream method depends on a class
// loader. No native classpath resource service exists yet; this counterpart's
// public methods are represented, but that private fidelity gap remains open.
}
