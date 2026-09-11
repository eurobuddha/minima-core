#include "support/NativeH2Connection.hpp"
#include <barrier>
#include <iostream>
#include <thread>
#include <vector>
using namespace minima;
int main(){try{
    NativeH2Runtime engine;auto connect=engine.connector();
    const std::string url="jdbc:h2:mem:workers;DB_CLOSE_ON_EXIT=FALSE";
    {
        auto owner=connect(url,"SA","");owner->createStatement()->execute("CREATE TABLE T(ID INT PRIMARY KEY, TXT VARCHAR)");
        std::barrier start(8);std::vector<std::jthread> workers;std::vector<std::exception_ptr> errors(8);
        for(int worker=0;worker<8;++worker)workers.emplace_back([&,worker]{
            start.arrive_and_wait();
            try{
                auto connection=connect(url,"SA","");auto insert=connection->prepareStatement("INSERT INTO T VALUES(?,?)");
                for(int i=0;i<64;++i){int id=worker*64+i;insert->setInt(1,id);insert->setString(2,std::to_string(id)+" π");if(insert->executeUpdate()!=1)throw std::runtime_error("Missing insertion");}
                // A child must retain the connection after the local owner dies.
                auto statement=connection->createStatement();connection.reset();auto rows=statement->executeQuery("SELECT TXT FROM T WHERE ID="+std::to_string(worker*64));statement.reset();
                if(!rows->next()||rows->getString("TXT")!=std::to_string(worker*64)+" π")throw std::runtime_error("Child lost its database owner");
            }catch(...){errors[worker]=std::current_exception();}
        });
        workers.clear();for(const auto& error:errors)if(error)std::rethrow_exception(error);
        auto rows=owner->createStatement()->executeQuery("SELECT COUNT(*) AS N,SUM(ID) AS TOTAL FROM T");
        if(!rows->next()||rows->getInt("N")!=512||rows->getLong("TOTAL")!=130816)throw std::runtime_error("Concurrent rows lost or duplicated");
        owner->close();
    }
    if(engine.liveHandles()!=0)throw std::runtime_error("Native handles leaked after concurrent ownership release");
    // A connector is independently usable after its public runtime owner dies.
    auto retained=NativeH2Runtime().connector();auto connection=retained("jdbc:h2:mem:retained","SA","");
    auto rows=connection->createStatement()->executeQuery("SELECT 42 AS N");
    if(!rows->next()||rows->getInt("N")!=42)throw std::runtime_error("Connector lost isolate ownership");
    connection->close();std::cout<<"PASS: 8 workers, 512 durable-in-isolate inserts, child and connector ownership, zero retained handles\n";
}catch(const std::exception& error){std::cerr<<sqlExceptionText(error)<<'\n';return 1;}}
