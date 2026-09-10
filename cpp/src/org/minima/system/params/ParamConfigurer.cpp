#include "ParamConfigurer.hpp"
#include "TestParams.hpp"
#include "GeneralParams.hpp"
#include "org/minima/system/network/p2p/params/P2PParams.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/utils/RPCClient.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "support/JavaInteger.hpp"
#include "support/JavaLowerCase.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaIOExceptionText.hpp"
#include "support/StrictUTF8CharacterReader.hpp"
#include "support/SocketCharacterReader.hpp"
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

namespace minima {
namespace {
using Failure=ParamConfigurer::ConfigurationException;
std::string utf8(const std::u16string& text){return unicode::encodeUTF8(text);}
const std::u16string& required(const ParamConfigurer::String& text) {
    if(!text)throw JavaNullPointerException("null parameter string");return *text;
}
std::u16string trim(const std::u16string& text) {
    size_t first=0,last=text.size();while(first<last&&text[first]<=32)++first;
    while(last>first&&text[last-1]<=32)--last;return text.substr(first,last-first);
}
void makeDirectories(const std::filesystem::path& path) {
    (void)javaUnixMkdirs(path); // Source ignores the boolean result.
}
std::string mapKey(size_t index) {
    // Enum.compareTo breaks equal-hash tree ties by ordinal, not enum name.
    auto ordinal=std::to_string(index);return std::string(8-ordinal.size(),'0')+ordinal;
}
uint32_t identityHash(size_t index) {
    static const auto hashes=[](){
        std::vector<uint32_t> result;
        for(size_t i=0;i<ParamConfigurer::keys().size();++i) {
            auto bytes=MiniData::getRandomData(4);uint32_t hash=0;
            for(auto byte:bytes.bytes())hash=(hash<<8)|byte;
            result.push_back(hash&0x7fffffffu);
        }
        return result;
    }();
    return hashes.at(index);
}
// Sequential Files.lines UTF-8 traversal. Opening errors are checked IO errors;
// decoder/read errors encountered during traversal are UncheckedIOException.
template<class Consumer> void lines(const std::filesystem::path& file,Consumer&& consume) {
    if(file.string().find('\0')!=std::string::npos)
        throw Failure("java.nio.file.InvalidPathException","Nul character not allowed: "+file.string());
    struct InputFile {
        int descriptor;
        ~InputFile(){if(descriptor>=0)::close(descriptor);}
    } input{::open(file.c_str(),O_RDONLY)};
    if(input.descriptor<0)throw wire::IOException("Unable to open configuration file");
    struct stat info{};
    if(::fstat(input.descriptor,&info)<0)throw wire::IOException(std::strerror(errno));
    // The source file-channel spliterator captures an int-sized file fence.
    std::optional<uint64_t> remaining;
    if(info.st_size>0&&info.st_size<=INT32_MAX)remaining=uint64_t(info.st_size);
    StrictUTF8CharacterReader decoder([&](std::span<uint8_t> bytes)->int32_t {
        if(remaining&&*remaining==0)return -1;
        if(remaining&&*remaining<bytes.size())bytes=bytes.first(size_t(*remaining));
        ssize_t count;
        do {count=::read(input.descriptor,bytes.data(),bytes.size());}while(count<0&&errno==EINTR);
        if(count<0)throw wire::IOException(std::strerror(errno));
        if(remaining)*remaining-=uint64_t(count);
        return count==0?-1:static_cast<int32_t>(count);
    });
    SocketCharacterReader reader([&](){return decoder.read();});
    for(;;) {
        std::optional<std::u16string> line;
        try {line=reader.readLineUTF16();}
        catch(const wire::IOException& error){throw Failure("java.io.UncheckedIOException",javaIOExceptionText(error));}
        if(!line)break;
        consume(*line);
    }
}
}
const std::vector<ParamConfigurer::ParamKey>& ParamConfigurer::keys() {
    static const std::vector<ParamKey> values{
#include "ParamConfigurerKeys.inc"
    };return values;
}
ParamConfigurer::ParamConfigurer(Runtime runtime):mRuntime(std::move(runtime)) {
    if(!mRuntime.enumHash)mRuntime.enumHash=identityHash;
    if(!mRuntime.println)mRuntime.println=[](const std::string& text){std::cout<<text<<std::endl;};
    if(!mRuntime.exit)mRuntime.exit=[](int status){std::exit(status);};
}
std::optional<size_t> ParamConfigurer::toParamKey(const std::u16string& text) {
    for(size_t i=0;i<keys().size();++i)if(text==unicode::decodeUTF8(keys()[i].key))return i;
    return std::nullopt;
}
std::optional<size_t> ParamConfigurer::progArgsToParamKey(const String& text) {
    const auto& value=required(text);
    if(!value.starts_with(u"-"))return std::nullopt;
    auto key=toParamKey(value.substr(1));
    if(!key)throw UnknownArgumentException(value);return key;
}
void ParamConfigurer::put(size_t key,std::u16string value) {
    if(!mParameters.putHashedKey(mapKey(key),mRuntime.enumHash(key),Entry{key,std::move(value)}))++mModCount;
}
void ParamConfigurer::putAll(const JavaHashMap<Entry>& entries) {
    mParameters.reserveForPutAll(entries.size());
    for(const auto& entry:entries.elements())put(entry.key,entry.value);
}
ParamConfigurer& ParamConfigurer::usingProgramArgs(const Args* args) {
    if(!args)throw JavaNullPointerException("null argument array");
    for(size_t index=0;index<args->size();++index) {
        if(auto key=progArgsToParamKey((*args)[index])) {
            std::u16string value=u"true";
            if(index+1<args->size()&&!progArgsToParamKey((*args)[index+1]))value=(*args)[index+1].value_or(u"true");
            put(*key,std::move(value));
        }
    }
    return *this;
}
ParamConfigurer& ParamConfigurer::usingConfFile(const Args* args) {
    if(!args)throw JavaNullPointerException("null argument array");
    size_t index=0;while(index<args->size()&&(*args)[index]!=String(u"-conf"))++index;
    if(index==args->size())return *this;
    if(++index==args->size())throw JavaArrayIndexOutOfBoundsException("Index "+std::to_string(index)+" out of bounds for length "+std::to_string(args->size()));
    auto file=*javaUnixFile(std::filesystem::path(utf8(required((*args)[index]))));
    try {
        JavaHashMap<Entry> collected;
        lines(file,[&](const std::u16string& line){
            // String.split("=") discards trailing empty tokens, including all
            // tokens for a nonempty line consisting solely of '=' characters.
            if(!line.empty()&&line.find_first_not_of(u'=')==line.npos)
                throw JavaArrayIndexOutOfBoundsException("Index 0 out of bounds for length 0");
            auto equal=line.find(u'=');auto name=line.substr(0,equal);
            auto value=equal==line.npos?std::u16string{}:line.substr(equal+1,line.find(u'=',equal+1)-(equal+1));
            if(auto key=toParamKey(name)) {
                auto hash=mRuntime.enumHash(*key);
                if(auto old=collected.getHashedKey(mapKey(*key),hash))
                    throw Failure("java.lang.IllegalStateException","Duplicate key "+keys()[*key].name+" (attempted merging values "+utf8(old->value)+" and "+utf8(value)+")");
                collected.putHashedKey(mapKey(*key),hash,Entry{*key,std::move(value)});
            }
        });
        putAll(collected);
    }catch(const wire::IOException&) {mRuntime.println("Unable to read conf file.");mRuntime.exit(1);}
    return *this;
}
ParamConfigurer& ParamConfigurer::usingEnvVariables(const Environment* environment) {
    if(!environment)throw JavaNullPointerException("null environment map");
    JavaHashMap<std::pair<std::u16string,std::u16string>> lowered;
    for(const auto& [name,value]:*environment) {
        auto key=javaLowerCaseUTF16(required(name),mRuntime.language);
        if(!key.starts_with(u"minima_"))continue;
        key=key.substr(7);auto actual=value.value_or(u"");
        if(auto old=lowered.getUTF16Key(key))
            throw Failure("java.lang.IllegalStateException","Duplicate key "+utf8(key)+" (attempted merging values "+utf8(old->second)+" and "+utf8(actual)+")");
        lowered.putUTF16Key(key,{key,std::move(actual)});
    }
    JavaHashMap<Entry> collected;
    for(const auto& [name,value]:lowered.elements())if(auto key=toParamKey(name))
        collected.putHashedKey(mapKey(*key),mRuntime.enumHash(*key),Entry{*key,value});
    putAll(collected);return *this;
}
ParamConfigurer& ParamConfigurer::configure() {
    auto modification=mModCount;
    for(const auto& entry:mParameters.elements())apply(entry.key,entry.value);
    if(modification!=mModCount)throw Failure("java.util.ConcurrentModificationException","");
    if(GeneralParams::SHOW_PARAMS) {
        MinimaLogger::log("Config Parameters");
        for(const auto& entry:mParameters.elements())MinimaLogger::log(String(unicode::decodeUTF8(keys()[entry.key].name)+u":"+entry.value));
    }
    return *this;
}
void ParamConfigurer::apply(size_t key,const std::u16string& value) {
    switch(key) {
#include "ParamConfigurerConsumers.inc"
        default:throw JavaIllegalArgumentException("Invalid parameter key");
    }
}
bool ParamConfigurer::checkParams(const Args* args) {
    try {ParamConfigurer().usingProgramArgs(args).configure();}
    catch(const std::exception&){return false;}
    return true;
}
bool ParamConfigurer::checkParams(const String& text) {
    // Source tokenization occurs outside checkParams(String[])'s catch block.
    const auto& full=required(text);Args args;
    for(size_t start=0;start<full.size();) {
        auto end=full.find(u' ',start);if(end==full.npos)end=full.size();
        auto token=trim(full.substr(start,end-start));if(!token.empty())args.emplace_back(std::move(token));start=end+1;
    }
    return checkParams(&args);
}
}
