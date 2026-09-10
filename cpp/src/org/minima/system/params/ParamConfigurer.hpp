#pragma once
#include "support/JavaConcurrentMap.hpp"
#include "support/JavaExceptions.hpp"
#include <filesystem>
#include <functional>

namespace minima {
class ParamConfigurer {
    friend struct ParamConfigurerTestAccess;
public:
    using String=std::optional<std::u16string>;
    using Args=std::vector<String>;
    using Environment=std::vector<std::pair<String,String>>;
    struct ParamKey {std::string name,key,help;};
    class UnknownArgumentException:public JavaTypedException {
    public:explicit UnknownArgumentException(const std::u16string& arg):JavaTypedException("org.minima.system.params.ParamConfigurer$UnknownArgumentException","Unknown argument : "+unicode::encodeUTF8(arg)){}
    };
    class ConfigurationException:public JavaTypedException {
    public:using JavaTypedException::JavaTypedException;
    };
    struct Runtime {
        // Enum identity hashes are stable within a native process, as in Java.
        // A receiver can bind identity hashes when embedding another runtime.
        std::function<uint32_t(size_t)> enumHash;
        std::function<void(const std::string&)> println;
        std::function<void(int)> exit;
        std::string language;
    };
    explicit ParamConfigurer(Runtime runtime={});
    static const std::vector<ParamKey>& keys();
    ParamConfigurer& usingConfFile(const Args* args);
    ParamConfigurer& usingConfFile(const Args& args){return usingConfFile(&args);}
    ParamConfigurer& usingEnvVariables(const Environment* environment);
    ParamConfigurer& usingEnvVariables(const Environment& environment){return usingEnvVariables(&environment);}
    ParamConfigurer& usingProgramArgs(const Args* args);
    ParamConfigurer& usingProgramArgs(const Args& args){return usingProgramArgs(&args);}
    ParamConfigurer& configure();
    bool isDaemon() const{return mDaemon;}
    bool isShutDownHook() const{return mShutdownhook;}
    static bool checkParams(const Args* args);
    static bool checkParams(const Args& args){return checkParams(&args);}
    static bool checkParams(const String& text);
private:
    struct Entry {size_t key;std::u16string value;};
    Runtime mRuntime;
    JavaHashMap<Entry> mParameters;
    bool mDaemon=false,mShutdownhook=true;
    size_t mModCount=0;
    static std::optional<size_t> toParamKey(const std::u16string& text);
    static std::optional<size_t> progArgsToParamKey(const String& text);
    void put(size_t key,std::u16string value);
    void putAll(const JavaHashMap<Entry>& entries);
    void apply(size_t key,const std::u16string& value);
};
}
