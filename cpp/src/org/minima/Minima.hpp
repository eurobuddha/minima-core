#pragma once
#include "system/Main.hpp"
#include "system/params/ParamConfigurer.hpp"
#include "support/ConsoleInput.hpp"
#include <thread>
namespace minima {
class Minima {
public:
    using String=std::optional<std::u16string>;
    using Args=ParamConfigurer::Args;
    struct Runtime {
        MainServices services;
        ParamConfigurer::Runtime config;
        std::function<String()> userHome;
        std::function<ParamConfigurer::Environment()> environment;
        std::function<std::shared_ptr<Main>(MainServices)> createMain;
        std::function<std::shared_ptr<ConsoleInput>()> openInput;
        std::function<void(std::function<void()>)> addShutdownHook;
        std::function<void(const std::string&)> println;
        std::function<void(int64_t)> sleep;
    };
    Minima()=default;
    static std::shared_ptr<Main> getMain(){return Main::getInstance();}
    std::string runMinimaCMD(const String& input){return runMinimaCMD(input,true);}
    std::string runMinimaCMD(const String& input,bool pretty){return runMinimaCMD(input,pretty,u"0x00");}
    std::string runMinimaCMD(const String& input,bool pretty,const String& user);
    static int main(const Args* args,Runtime runtime);
    static int main(const Args* args){return main(args,Runtime{});}
    static int main(const Args& args,Runtime runtime){return main(&args,std::move(runtime));}
    static int main(const Args& args){return main(&args,Runtime{});}
    // The shared argument array remains visible to the worker. Native embedders
    // join starter threads before ending their host process.
    void mainStarter(std::shared_ptr<Args> args,Runtime runtime);
    void mainStarter(std::shared_ptr<Args> args){mainStarter(std::move(args),Runtime{});}
    static void joinStarterThreads();
private:
    inline static std::atomic<bool> mIsRunning{true};
    inline static std::mutex mInputLock;
    inline static std::shared_ptr<ConsoleInput> mInputStream,mBufferedStream;
    static std::shared_ptr<ConsoleInput> inputOwner(const std::shared_ptr<ConsoleInput>& input){std::lock_guard guard(mInputLock);return input;}
};
}
