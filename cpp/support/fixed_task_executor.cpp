#include "support/FixedTaskExecutor.hpp"
#include <atomic>
#include <vector>
using namespace minima;
static void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
static thread_local bool originalWorker=false;
struct State {
    std::mutex lock;
    std::condition_variable changed;
    int started=0,active=0,maximum=0;
    bool releaseFirst=false,releaseOthers=false,replacement=true;
    std::vector<int> completed;
};
int main(){try{
    bool rejected=false;try{FixedTaskExecutor invalid(0);}catch(const std::invalid_argument&){rejected=true;}require(rejected,"zero workers accepted");
    FixedTaskExecutor executor(4);
    require(!executor.awaitTermination(0),"unclosed empty pool reports terminated");
    rejected=false;try{executor.execute({});}catch(const std::invalid_argument&){rejected=true;}require(rejected,"null task accepted");
    auto state=std::make_shared<State>();
    for(int i=0;i<4;++i)executor.execute([state,i]{
        originalWorker=true;std::unique_lock lock(state->lock);++state->started;++state->active;state->maximum=std::max(state->maximum,state->active);state->changed.notify_all();
        state->changed.wait(lock,[&]{return i==0?state->releaseFirst:state->releaseOthers;});
        --state->active;lock.unlock();if(i==0)throw std::runtime_error("intentional worker failure");
    });
    {
        std::unique_lock lock(state->lock);require(state->changed.wait_for(lock,std::chrono::seconds(5),[&]{return state->started==4;}),"four workers did not start");
    }
    for(int i=0;i<100;++i)executor.execute([state,i]{std::lock_guard lock(state->lock);state->replacement=state->replacement&&!originalWorker;state->completed.push_back(i);state->changed.notify_all();});
    executor.shutdown();
    require(!executor.awaitTermination(25),"shutdown canceled held work");
    rejected=false;try{executor.execute([]{});}catch(const std::runtime_error&){rejected=true;}require(rejected,"work accepted after shutdown");
    {
        std::lock_guard lock(state->lock);require(state->completed.empty(),"queued tasks exceeded four-worker limit");state->releaseFirst=true;state->changed.notify_all();
    }
    {
        std::unique_lock lock(state->lock);require(state->changed.wait_for(lock,std::chrono::seconds(5),[&]{return state->completed.size()==100;}),"failed worker was not replaced during drain");
        require(state->replacement,"failed worker reused instead of replaced");
        for(int i=0;i<100;++i)require(state->completed[size_t(i)]==i,"single available worker queue is not FIFO");
        require(state->maximum==4,"worker count differs");state->releaseOthers=true;state->changed.notify_all();
    }
    require(executor.awaitTermination(5000),"drained executor did not terminate");
    executor.shutdown();require(executor.awaitTermination(0),"repeated shutdown changed termination");
    std::cout<<"four-workers; queued FIFO; timeout preserves tasks; shutdown rejection; failed-worker replacement; drain termination\n";
    // Captured state outlives the owning executor, as native task closures do.
    auto survivor=std::make_shared<State>();
    {
        FixedTaskExecutor temporary(1);
        temporary.execute([survivor]{std::unique_lock lock(survivor->lock);survivor->changed.wait(lock,[&]{return survivor->releaseFirst;});});
        for(int i=0;i<30;++i)temporary.execute([survivor,i]{std::lock_guard lock(survivor->lock);survivor->completed.push_back(i);survivor->changed.notify_all();});
    }
    {
        std::unique_lock lock(survivor->lock);survivor->releaseFirst=true;survivor->changed.notify_all();require(survivor->changed.wait_for(lock,std::chrono::seconds(5),[&]{return survivor->completed.size()==30;}),"destroyed executor lost tasks");
        for(int i=0;i<30;++i)require(survivor->completed[size_t(i)]==i,"destroyed owner queue order differs");
    }
    std::cout<<"owner destruction preserves queued task lifetimes\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
