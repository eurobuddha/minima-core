#include "Minima.hpp"
#include "system/commands/CommandRunner.hpp"
#include "system/params/GeneralParams.hpp"
#include "system/params/GlobalParams.hpp"
#include "utils/MiniFormat.hpp"
#include "utils/MinimaUncaughtException.hpp"
#include "support/JavaStringTrim.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/NativeShutdownHooks.hpp"
#include "support/NativeThrowable.hpp"
#include <bit>
#include <cstdlib>
#include <iostream>
#include <pwd.h>
#include <unistd.h>
#ifdef __APPLE__
#include <crt_externs.h>
#else
extern char** environ;
#endif
namespace minima {
namespace {
struct ProcessExit {int code;};
struct Starters {std::mutex lock;std::vector<std::thread> threads;};
Starters& starters(){static auto* value=new Starters();return *value;}
const std::u16string& required(const Minima::String& text){if(!text)throw JavaNullPointerException("null Minima command string");return *text;}
Minima::String home(){
    struct passwd entry{},*found=nullptr;long requested=sysconf(_SC_GETPW_R_SIZE_MAX);std::vector<char> buffer(requested>0?size_t(requested):16384);
    int error;while((error=getpwuid_r(getuid(),&entry,buffer.data(),buffer.size(),&found))==ERANGE)buffer.resize(buffer.size()*2);
    return !error&&found&&found->pw_dir?Minima::String(unicode::decodeUTF8(found->pw_dir)):std::nullopt;
}
ParamConfigurer::Environment environment(){
    ParamConfigurer::Environment result;
#ifdef __APPLE__
    char** values=*_NSGetEnviron();
#else
    char** values=environ;
#endif
    for(;values&&*values;++values){std::string value=*values;auto at=value.find('=');if(at!=value.npos)result.emplace_back(unicode::decodeUTF8(value.substr(0,at)),unicode::decodeUTF8(value.substr(at+1)));}return result;
}
}
std::string Minima::runMinimaCMD(const String& input,bool pretty,const String& user){
    (void)javaStringTrim(required(input)); // The source dispatches the original, untrimmed input.
    auto result=CommandRunner::getRunner().runMultiCommandNullableUser(user?std::optional(unicode::encodeUTF8(*user)):std::nullopt,unicode::encodeUTF8(*input));
    if(pretty)return MiniFormat::JSONPretty(result);
    if(result.size()==1){auto first=result.get(0);if(first.isNull())throw JavaNullPointerException("null command result");return first.isString()?first.asString():first.toJSONString();}
    return result.toJSONString();
}
void Minima::mainStarter(std::shared_ptr<Args> args,Runtime runtime){
    auto name=nextNativeThreadName();auto& registry=starters();std::lock_guard guard(registry.lock);
    if(!runtime.config.exit)runtime.config.exit=[](int code){std::exit(code);};
    if(!runtime.services.exit)runtime.services.exit=[](int code){std::exit(code);};
    registry.threads.emplace_back([args=std::move(args),runtime=std::move(runtime),name]() mutable {runNativeThread(NativeThread{name},[&]{int status=main(args.get(),std::move(runtime));if(status)std::exit(status);});});
}
void Minima::joinStarterThreads(){
    auto& registry=starters();
    for(;;){
        std::thread worker;
        {
            std::lock_guard guard(registry.lock);
            // A starter cannot wait for itself. Leave every owner in the
            // registry so an external host can still join after this error.
            for(const auto& thread:registry.threads)if(thread.get_id()==std::this_thread::get_id())
                throw JavaIllegalArgumentException("A starter thread cannot join starter threads");
            if(registry.threads.empty())return;
            worker=std::move(registry.threads.back());registry.threads.pop_back();
        }
        worker.join();
    }
}
int Minima::main(const Args* args,Runtime runtime){
    mIsRunning=true;
    if(!runtime.userHome)runtime.userHome=home;if(!runtime.environment)runtime.environment=environment;
    if(!runtime.println)runtime.println=[](const std::string& text){std::cout<<text<<std::endl;};
    if(!runtime.openInput)runtime.openInput=openStandardConsoleInput;
    if(!runtime.createMain)runtime.createMain=[](MainServices services){return Main::create(std::move(services));};
    if(!runtime.addShutdownHook)runtime.addShutdownHook=addNativeShutdownHook;
    if(!runtime.sleep)runtime.sleep=nativeThreadSleep;
    auto userHome=runtime.userHome();auto data=userHome?javaUnixChildFile(unicode::encodeUTF8(*userHome),".minima"):std::filesystem::path(".minima");
    auto folder=javaUnixChildFile(data,GlobalParams::MINIMA_BASE_VERSION);GeneralParams::resetDefaults();GeneralParams::DATA_FOLDER=javaUnixAbsoluteFile(folder);
    auto configRuntime=runtime.config;if(!configRuntime.println)configRuntime.println=runtime.println;
    if(!configRuntime.exit)configRuntime.exit=[](int code){throw ProcessExit{code};};
    try{
        ParamConfigurer configurer(configRuntime);
        try{configurer.usingConfFile(args).usingEnvVariables(runtime.environment()).usingProgramArgs(args).configure();}
        catch(const ParamConfigurer::UnknownArgumentException& error){runtime.println(error.toString());return 1;}
        javaUnixMkdirs(GeneralParams::DATA_FOLDER);
        bool daemon=configurer.isDaemon(),shutdownhook=configurer.isShutDownHook();
        auto port=[](int32_t delta){return std::bit_cast<int32_t>(uint32_t(GeneralParams::MINIMA_PORT)+uint32_t(delta));};
        GeneralParams::MDSFILE_PORT=port(2);GeneralParams::MDSCOMMAND_PORT=port(3);GeneralParams::RPC_PORT=port(4);
        for(const char* line:{"**********************************************","*  __  __  ____  _  _  ____  __  __    __    *","* (  \\/  )(_  _)( \\( )(_  _)(  \\/  )  /__\\   *","*  )    (  _)(_  )  (  _)(_  )    (  /(__)\\  *","* (_/\\/\\_)(____)(_)\\_)(____)(_/\\/\\_)(__)(__) *","*                                            *","**********************************************"})MinimaLogger::log(line);
        MinimaLogger::log("Welcome to Pure Minima "+GlobalParams::getFullMicroVersion()+" - for assistance type help. Then press enter.");
        setDefaultUncaughtExceptionHandler(std::make_shared<MinimaUncaughtException>(runtime.services.halt));
        // Native database providers are supplied through MainServices; no Java VM
        // or recording database is silently installed by the executable.
        if(!runtime.services.exit)runtime.services.exit=[](int code){throw ProcessExit{code};};
        auto main=runtime.createMain(std::move(runtime.services));sqlRequired(main);
        if(shutdownhook)runtime.addShutdownHook([main]{if(main->isShuttingDown())return;MinimaLogger::log("[!] Shutdown Hook..");main->shutdown();});
        if(daemon){MinimaLogger::log("Daemon mode started..");while(!main->isShutdownComplete())try{runtime.sleep(250);}catch(const JavaTypedException& error){if(error.className()!="java.lang.InterruptedException")throw;}return 0;}
        auto input=runtime.openInput();
        {std::lock_guard guard(mInputLock);mInputStream=input;mBufferedStream=std::move(input);}
        while(mIsRunning&&main->isRunning()){
            try{
                auto input=sqlRequired(inputOwner(mBufferedStream)).readLine();
                if(input&&!input->empty()){
                    auto text=javaStringTrim(*input);if(!Main::getInstance())break;
                    auto result=CommandRunner::getRunner().runMultiCommand(unicode::encodeUTF8(text));
                    if(text!=u"quit")runtime.println(MiniFormat::JSONPretty(result));
                    bool quit=false;for(const auto& value:result.elements()){
                        if(value.isNull())throw JavaNullPointerException("null command result");auto object=std::get_if<std::shared_ptr<json::JSONObject>>(&value.value);if(!object)throw JavaClassCastException("command result is not JSONObject");
                        auto command=(**object).get("command");if(command.isNull())throw JavaNullPointerException("null command name");if(command.isString()&&command.asUTF16()==u"quit"){quit=true;break;}
                    }if(quit)break;
                }
            }catch(const JavaError&){throw;}catch(const std::bad_alloc&){throw;}catch(const std::exception& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
        }
        try{sqlRequired(inputOwner(mBufferedStream)).closeBufferedReader();sqlRequired(inputOwner(mInputStream)).closeInputStreamReader();}
        catch(const wire::IOException& error){MinimaLogger::log(javaIOExceptionText(error));}
        MinimaLogger::log("Minima CLI input stopped.. ",false);return 0;
    }catch(const ProcessExit& exit){return exit.code;}
}
}
