// Friend fixture access to the three source-private formatting helpers.
#include "org/minima/system/commands/Command.hpp"
#include "org/minima/system/commands/base/benchmark.hpp"
#include "org/minima/system/commands/base/jniminingtest.hpp"
#include "org/minima/system/commands/base/jnitest.hpp"
#include "support/JNICommandRuntime.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
namespace minima::commands {
class JNICommandFixture {public:static std::string format(const std::string& name,const std::vector<uint8_t>& bytes){if(name=="benchmark")return benchmark::outputByteArray(bytes);if(name=="jnitest")return jnitest::outputByteArray(bytes);return jniminingtest::outputByteArray(bytes);}};
}
static JSONObject& object(JSONValue& v){return *std::get<std::shared_ptr<JSONObject>>(v.value);}
int main(int argc,char** argv) {
    if(argc!=2)return 2;
    auto input=JSONValue::parseWithException(argv[1]);auto& request=object(input);
    auto name=request.get("name").asString(),mode=request.get("mode").asString();
    std::shared_ptr<Command> command;
    if(name=="benchmark")command=std::make_shared<commands::benchmark>();
    else if(name=="jnitest")command=std::make_shared<commands::jnitest>();
    else if(name=="jniminingtest")command=std::make_shared<commands::jniminingtest>();else return 2;
    auto params=request.get("params");*command->getParams()=object(params);
    JSONObject events,out;
    if(mode!="unloaded") {
        JNICommandRuntime::sayHello=[&]{events.put("hello",true);if(mode=="hello_error")throw JavaTypedException("java.lang.IllegalStateException","fixture hello error");};
        JNICommandRuntime::hashHeaderWithDiff=[&](JNICommandRuntime::Buffer nonce,int32_t attempts,JNICommandRuntime::Buffer difficulty,JNICommandRuntime::Buffer header)->std::optional<wire::Bytes> {
            events.put("nonce",MiniData::fromBytes(nonce.data(),nonce.size()).to0xString());events.put("attempts",attempts);
            events.put("difficulty",MiniData::fromBytes(difficulty.data(),difficulty.size()).to0xString());events.put("header",MiniData::fromBytes(header.data(),header.size()).to0xString());
            if(mode=="hash_error")throw JavaTypedException("java.lang.IllegalStateException","fixture hash error");
            if(request.containsKey("mutate")) {
                std::fill(difficulty.begin(),difficulty.end(),uint8_t(std::get<int64_t>(request.get("mutate").value)));
                events.put("mutateddifficulty",MiniData::fromBytes(difficulty.data(),difficulty.size()).to0xString());
            }
            if(request.containsKey("result")){auto result=request.get("result");if(result.isNull())return std::nullopt;return MiniData::fromHex(result.asString()).bytes();}
            return wire::Bytes(nonce.begin(),nonce.end());
        };
    }
    try {
        if(mode=="meta") {
            JSONObject meta;meta.put("name",*command->getName());meta.put("help",*command->getHelp());meta.put("fullhelp",*command->getFullHelp());
            JSONArray valid;for(auto& p:command->getValidParams())valid.add(p);meta.put("validparams",valid);
            auto fresh=command->getFunction();meta.put("freshname",*fresh->getName());meta.put("freshparams",*fresh->getParams());meta.put("distinct",fresh!=command);
            std::vector<uint8_t> bytes(256);for(int i=0;i<256;++i)bytes[size_t(i)]=uint8_t(i);
            auto formatted=commands::JNICommandFixture::format(name,bytes);
            meta.put("byteformat",formatted);out.put("value",meta);
        }else out.put("value",command->runCommand());
    }catch(const CommandException& e){out.put("error","org.minima.system.commands.CommandException");out.put("message",e.what());}
    catch(const wire::EOFException&){out.put("error","java.io.EOFException");out.put("message",JSONValue());}
    catch(const wire::IOException& e){out.put("error","java.io.IOException");out.put("message",e.what());}
    catch(const JavaTypedException& e){out.put("error",e.className());out.put("message",e.what());}
    catch(const std::exception& e){auto text=javaExceptionText(e);auto split=text.find(": ");out.put("error",text.substr(0,split));out.put("message",split==std::string::npos?JSONValue():JSONValue(text.substr(split+2)));}
    out.put("events",events);std::cout<<out.toJSONString()<<'\n';
}
