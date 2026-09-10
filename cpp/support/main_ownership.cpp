#include "org/minima/system/Main.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/utils/messages/TimerProcessor.hpp"
#include "support/NativeThread.hpp"
#include <filesystem>
#include <iostream>
#include <unistd.h>
using namespace minima;
namespace {
void require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
class Capture:public NativeUncaughtExceptionHandler {public:std::string name;void uncaughtException(const NativeThread*,const std::shared_ptr<MinimaLogger::Throwable>&) override{name=currentNativeThreadName();}};
class RetiredListener:public MessageListener {bool& mReleased;public:explicit RetiredListener(bool& released):mReleased(released){}~RetiredListener(){mReleased=!Main::getMinimaListener();}void processMessage(std::shared_ptr<Message>) override{}};
}
int main(){try{
    require(currentNativeThreadName()=="main","initial name");auto capture=std::make_shared<Capture>();auto previous=getDefaultUncaughtExceptionHandler();setDefaultUncaughtExceptionHandler(capture);
    bool outer=false,inner=false;runNativeThread(NativeThread{u"OUTER"},[&]{outer=currentNativeThreadName()=="OUTER";runNativeThread(NativeThread{u"INNER"},[&]{inner=currentNativeThreadName()=="INNER";throw JavaCheckedException("fixture");});require(currentNativeThreadName()=="OUTER","nested name restoration");});setDefaultUncaughtExceptionHandler(previous);
    require(outer&&inner&&capture->name=="INNER"&&currentNativeThreadName()=="main","worker name propagation");
    bool listenerReleased=false;Main::setMinimaListener(std::make_shared<RetiredListener>(listenerReleased));Main::setMinimaListener(nullptr);require(listenerReleased,"listener destructor could not reenter registry");
    std::string pattern=(std::filesystem::temp_directory_path()/"minima-main-startup-XXXXXX").string();std::vector<char> buffer(pattern.begin(),pattern.end());buffer.push_back(0);auto directory=mkdtemp(buffer.data());require(directory,"private startup directory");std::filesystem::path root(directory);
    struct Cleanup{std::filesystem::path path;~Cleanup(){std::error_code error;std::filesystem::remove_all(path,error);}}cleanup{root};
    GeneralParams::resetDefaults();GeneralParams::DATA_FOLDER=root.string();GeneralParams::IS_MOBILE=false;GeneralParams::IS_JNLP=false;GeneralParams::RESCUE_MEGAMMR_NODE="";GeneralParams::LOAD_JNI=false;GeneralParams::CLEAN=false;GeneralParams::P2P_ENABLED=false;GeneralParams::NOCONNECT=true;
    MinimaLogger::Runtime logger;logger.println=[](const auto&){};MinimaLogger::setRuntime(logger);
    std::weak_ptr<Main> seen;int connections=0,halt=-1;MainServices services;services.sql.log=[](const auto&,bool){};
    services.sql.connect=[&](const auto&,const auto&,const auto&)->std::shared_ptr<SqlConnection>{++connections;seen=Main::getInstance();require(!seen.expired(),"Main published before SQL");require(MinimaDB::currentWriteLockThread()=="main","database thread diagnostic");throw SqlException("fixture unavailable database");};
    services.halt=[&](int code){halt=code;throw JavaError("fixture.StopStartup","halt");};
    services.exit=services.halt;
    services.loadNativeLibrary=[&]{seen=Main::getInstance();throw JavaError("java.lang.UnsatisfiedLinkError","fixture");};
    GeneralParams::LOAD_JNI=true;bool earlyFailed=false;try{Main::create(services);}catch(const JavaError& error){earlyFailed=error.toString()=="fixture.StopStartup: halt";}
    require(earlyFailed&&connections==0&&halt==1&&!Main::getInstance()&&seen.expired(),"failure before timer creation leaked Main");require(!TimerProcessor::getTimerProcessor(),"early failure created timer");GeneralParams::LOAD_JNI=false;
    bool failed=false;try{Main::create(services);}catch(const JavaError& error){failed=error.toString()=="fixture.StopStartup: halt";}
    require(failed&&connections==1&&halt==0,"startup failure boundary");require(!Main::getInstance()&&seen.expired(),"failed startup retained Main worker");auto timers=TimerProcessor::getTimerProcessor();require(bool(timers),"startup created timer owner");timers->join();MinimaDB::getDB()->safeReleaseReadWriteLock();
    std::cout<<"Main publication, failed-startup ownership and nested worker names passed\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
