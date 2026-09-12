#include "org/minima/database/cascade/Cascade.hpp"
#include "org/minima/objects/IBD.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/FixedTaskExecutor.hpp"
#include <atomic>
#include <future>
#include <pthread.h>
#include <stdexcept>
#include <thread>
#include <type_traits>

using namespace minima;
namespace {
constexpr size_t depth=30000;
void require(bool value,const char* why){if(!value)throw std::runtime_error(why);}
using Node=std::shared_ptr<CascadeNode>;
using Witnesses=std::vector<std::weak_ptr<CascadeNode>>;

CascadeNode prototype(){
    TxPoW tx;tx.setTimeMilli(MiniNumber(1789160000000LL));tx.setNonce(MiniNumber(7));
    tx.setBlockNumber(MiniNumber(123));tx.clearBody();tx.calculateTXPOWID();
    return CascadeNode(tx);
}
Node chain(size_t count,Witnesses& witnesses,Node tail={}){
    // Public copy construction shares the immutable TxPoW fixture's payload;
    // each CascadeNode and each parent edge is distinct and uses production code.
    auto sample=prototype();
    for(size_t i=0;i<count;++i){auto node=std::make_shared<CascadeNode>(sample);node->setLevel(int32_t(i%31));node->setParent(std::move(tail));tail=std::move(node);witnesses.emplace_back(tail);}
    return tail;
}
void expired(const Witnesses& witnesses){for(const auto& weak:witnesses)require(weak.expired(),"unreachable cascade node retained");}
size_t inspect(Node node){size_t count=0;for(;node;node=node->getParent()){require(node->getTxPoW().getBlockNumber().isEqual(MiniNumber(123)),"shared payload changed");++count;}return count;}
struct Worker {std::function<void()> task;std::exception_ptr failure;};
void* pthreadBody(void* argument){auto& worker=*static_cast<Worker*>(argument);try{worker.task();}catch(...){worker.failure=std::current_exception();}return nullptr;}
void smallWorker(std::function<void()> task){
    Worker worker{std::move(task),{}};pthread_attr_t attributes;
    require(pthread_attr_init(&attributes)==0,"pthread attributes");
    require(pthread_attr_setstacksize(&attributes,512*1024)==0,"512KiB worker stack");
    pthread_t thread;int result=pthread_create(&thread,&attributes,pthreadBody,&worker);pthread_attr_destroy(&attributes);
    require(result==0,"pthread creation");require(pthread_join(thread,nullptr)==0,"pthread join");
    if(worker.failure)std::rethrow_exception(worker.failure);
}
void executorWorker(std::function<void()> task){
    FixedTaskExecutor executor(1);std::promise<void> completed;auto future=completed.get_future();
    executor.execute([&]{try{task();completed.set_value();}catch(...){completed.set_exception(std::current_exception());}});
    executor.shutdown();require(executor.awaitTermination(600000),"cascade executor did not finish");future.get();
}
void uniqueChain(){Witnesses witnesses;auto root=chain(depth,witnesses);root.reset();expired(witnesses);}
void threadExit(){
    Witnesses witnesses;
    smallWorker([&]{thread_local Node lastOwner;lastOwner=chain(depth,witnesses);});
    expired(witnesses);
}
void sharedTail(){
    Witnesses tailWitnesses,firstWitnesses,secondWitnesses;
    auto tail=chain(depth/2,tailWitnesses);auto bytes=tail->serialise();
    auto first=chain(depth/2,firstWitnesses,tail);auto second=chain(depth/2,secondWitnesses,tail);tail.reset();
    first.reset();expired(firstWitnesses);
    auto survivor=tailWitnesses.back().lock();require(bool(survivor),"shared tail destroyed early");
    require(inspect(survivor)==depth/2&&survivor->serialise()==bytes,"shared tail was detached or changed");
    survivor.reset();second.reset();expired(secondWitnesses);expired(tailWitnesses);
}
void retainedWeakOwner(){
    Witnesses witnesses;auto root=chain(depth,witnesses);auto weak=witnesses.at(depth/2);
    std::promise<void> acquired,release;auto gate=release.get_future();std::exception_ptr failure;
    std::thread observer([&]{try{auto held=weak.lock();require(bool(held),"weak promotion failed");acquired.set_value();gate.wait();require(inspect(held)==depth/2+1,"promoted owner lost its parents");}catch(...){failure=std::current_exception();}});
    acquired.get_future().wait();root.reset();release.set_value();observer.join();if(failure)std::rethrow_exception(failure);expired(witnesses);
}
void concurrentWeakPromotions(){
    Witnesses witnesses;auto root=chain(depth,witnesses);auto weak=witnesses.at(depth/2);
    std::promise<void> ready;std::atomic<bool> released=false;std::exception_ptr failure;
    std::thread observer([&]{try{ready.set_value();do{if(auto held=weak.lock()){auto parent=held->getParent();require(bool(parent),"live weak owner observed severed parent");require(parent->getLevel()==int32_t((depth/2-1)%31),"live parent changed");}}while(!released.load());}catch(...){failure=std::current_exception();}});
    ready.get_future().wait();root.reset();released=true;observer.join();if(failure)std::rethrow_exception(failure);expired(witnesses);
}
std::shared_ptr<Cascade> cascade(Witnesses& witnesses){
    auto result=std::make_shared<Cascade>();auto sample=prototype();result->addToTip(sample.getTxPoW());
    result->getTip()->setParent(chain(depth-1,witnesses));witnesses.emplace_back(result->getTip());return result;
}
void temporaryIBD(){Witnesses witnesses;{IBD temporary;temporary.setCascade(cascade(witnesses));require(temporary.getCascade()->getLength()==depth,"IBD fixture length");}expired(witnesses);}
void sharedIBD(){
    Witnesses witnesses;auto retained=cascade(witnesses);
    {IBD temporary;temporary.setCascade(retained);IBD another=temporary;require(another.getCascade()==retained,"IBD copy lost shared cascade");}
    require(retained->getLength()==depth,"IBD destruction changed surviving cascade");retained.reset();expired(witnesses);
}
void completeIBD(){
    Witnesses originalWitnesses,copyWitnesses;auto original=cascade(originalWitnesses);
    auto oldServices=IBD::services;auto oldQuota=GlobalParams::MINIMA_CASCADE_LEVEL_NODES;
    struct Restore{IBDServices services;int32_t quota;~Restore(){IBD::services=std::move(services);GlobalParams::MINIMA_CASCADE_LEVEL_NODES=quota;}} restore{oldServices,oldQuota};
    GlobalParams::MINIMA_CASCADE_LEVEL_NODES=int32_t(depth+1);
    IBD::services.getCascade=[original]{return original;};IBD::services.getTree=[]{return std::make_shared<TxPowTree>();};
    {IBD temporary;temporary.createCompleteIBD();require(temporary.getCascade()->getLength()==depth,"complete IBD copy shortened unexpectedly");
     require(temporary.getCascade()->getTip()!=original->getTip(),"complete IBD did not deep copy");
     for(auto node=temporary.getCascade()->getTip();node;node=node->getParent())copyWitnesses.emplace_back(node);}
    expired(copyWitnesses);require(original->getLength()==depth,"temporary complete IBD changed original");
    IBD::services=oldServices;original.reset();expired(originalWitnesses);
}
void replacementAndAssignment(){
    Witnesses oldWitnesses,newWitnesses;auto root=chain(depth,oldWitnesses);auto replacement=chain(depth,newWitnesses);
    auto sample=prototype();sample.setParent(root);root.reset();sample.setParent(replacement);expired(oldWitnesses);
    CascadeNode copy=sample;sample.setParent(nullptr);require(inspect(copy.getParent())==depth,"node copy lost its parent");
    CascadeNode moved=std::move(copy);require(!copy.getParent(),"node move copied parent");
    sample=std::move(moved);require(!moved.getParent(),"node move assignment copied parent");
    replacement.reset();sample=prototype();expired(newWitnesses);
    Witnesses cascadeWitnesses;auto old=cascade(cascadeWitnesses);*old=Cascade{};expired(cascadeWitnesses);
}
void reentrantDeleter(){
    Witnesses firstWitnesses,secondWitnesses;auto sample=prototype();
    auto sibling=std::make_shared<CascadeNode>(sample);sibling->setParent(chain(depth,secondWitnesses));
    Node custom(new CascadeNode(sample),[sibling=std::move(sibling)](CascadeNode* node)mutable{sibling.reset();delete node;});
    custom->setParent(chain(depth,firstWitnesses));auto root=std::make_shared<CascadeNode>(sample);root->setParent(std::move(custom));root.reset();expired(firstWitnesses);expired(secondWitnesses);
}
}
int main(int argc,char** argv){try{
    static_assert(std::is_copy_constructible_v<CascadeNode> && std::is_move_constructible_v<CascadeNode>);
    static_assert(std::is_copy_assignable_v<CascadeNode> && std::is_move_assignable_v<CascadeNode>);
    const std::vector<std::pair<std::string,void(*)()>> cases={{"unique-chain",uniqueChain},{"thread-exit",threadExit},{"shared-tail",sharedTail},{"retained-weak-owner",retainedWeakOwner},{"concurrent-weak-promotions",concurrentWeakPromotions},{"temporary-ibd",temporaryIBD},{"shared-ibd",sharedIBD},{"complete-ibd",completeIBD},{"replacement-and-assignment",replacementAndAssignment},{"reentrant-deleter",reentrantDeleter}};
    size_t count=0;for(const auto& [name,test]:cases){if(argc>1&&name!=argv[1])continue;std::cout<<"START "<<name<<std::endl;smallWorker(test);executorWorker(test);++count;std::cout<<"PASS "<<name<<" (512KiB pthread + FixedTaskExecutor)"<<std::endl;}
    require(count>0,"unknown regression case");std::cout<<"{\"passed\":true,\"cases\":"<<count<<",\"worker_runs\":"<<2*count<<",\"chain_depth\":"<<depth<<",\"small_stack_bytes\":524288}"<<std::endl;
}catch(const std::exception& error){std::cerr<<"FAIL "<<error.what()<<std::endl;return 1;}}
