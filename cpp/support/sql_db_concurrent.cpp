#include "org/minima/utils/SqlDB.hpp"
#include <atomic>
#include <barrier>
#include <iostream>
#include <thread>
#include <vector>
using namespace minima;
namespace {
struct State {
    std::atomic<int> active=0, statements=0, schemas=0, connects=0, failures=0;
};
class Statement:public SqlStatement {
    State& state;
public:
    explicit Statement(State& value):state(value) {
        if(state.active.fetch_add(1)!=0)++state.failures;
        ++state.statements;
    }
    bool execute(const std::optional<std::string>&) override {
        for(int i=0;i<8;++i)std::this_thread::yield();
        return false;
    }
    std::shared_ptr<SqlResultSet> executeQuery(const std::optional<std::string>&) override {throw SqlException("unused fixture query");}
    std::shared_ptr<SqlResultSet> getResultSet() override {throw SqlException("unused fixture results");}
    void close() override {if(state.active.fetch_sub(1)!=1)++state.failures;}
};
class Connection:public SqlConnection {
    State& state;
    bool closed=false;
public:
    explicit Connection(State& value):state(value){}
    bool isClosed() override {return closed;}
    void setAutoCommit(bool enabled) override {if(!enabled)++state.failures;}
    std::shared_ptr<SqlStatement> createStatement() override {return std::make_shared<Statement>(state);}
    std::shared_ptr<SqlPreparedStatement> prepareStatement(const std::string&) override {throw SqlException("unused fixture preparation");}
    void close() override {closed=true;}
};
class DB:public SqlDB {
    State& state;
    void createSQL() override {
        ++state.schemas;
        // Reopening through executeGenericSQL already holds the monitor.
        // A synchronized subclass operation must be able to reenter it.
        if(!executeGenericSQL("schema probe").get("status").asBool())++state.failures;
    }
public:
    explicit DB(State& value):SqlDB({[&value](const auto&,const auto&,const auto&){++value.connects;return std::make_shared<Connection>(value);},[]{return false;},[](const auto&,bool){}}),state(value){}
};
}
int main(int argc,char** argv) {
    if(argc!=2)return 2;
    State state;DB db(state);db.loadDB(std::filesystem::path(argv[1]));db.hardCloseDB();
    std::barrier start(8);std::vector<std::thread> workers;
    for(int i=0;i<8;++i)workers.emplace_back([&]{
        start.arrive_and_wait();
        for(int n=0;n<128;++n)try {
            if(!db.executeGenericSQL("concurrent probe").get("status").asBool())++state.failures;
        } catch(const std::exception&) {++state.failures;}
    });
    for(auto& worker:workers)worker.join();
    db.hardCloseDB();
    bool passed=state.active==0&&state.statements==1026&&state.schemas==2&&state.connects==2&&state.failures==0;
    std::cout<<"SqlDB serialized 1024 concurrent calls and two reentrant schema calls: "<<(passed?"PASS":"FAIL")<<'\n';
    return passed?0:1;
}
