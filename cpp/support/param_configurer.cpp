#include "org/minima/system/params/ParamConfigurer.hpp"
#include "org/minima/system/params/TestParams.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "org/minima/system/network/p2p/params/P2PParams.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
#include "support/NativeThrowable.hpp"
#include <sstream>
#include <iostream>
using namespace minima;
using namespace minima::json;
#include "ParamSnapshots.hpp"
namespace minima {
struct ParamConfigurerTestAccess {
    static JSONArray entries(const ParamConfigurer& config) {JSONArray out;for(const auto& entry:config.mParameters.elements()){JSONArray row;row.add(ParamConfigurer::keys()[entry.key].name);row.add(entry.value);out.add(row);}return out;}
};
}
struct Exit {int status;};
static ParamConfigurer::String string(const JSONValue& value){return value.isNull()?std::nullopt:std::optional(value.asUTF16());}
static std::optional<ParamConfigurer::Args> args(const JSONValue& value){if(value.isNull())return {};ParamConfigurer::Args result;for(const auto& item:std::get<std::shared_ptr<JSONArray>>(value.value)->elements())result.push_back(string(item));return result;}
static std::string errorText(const std::exception& error){return nativeExceptionText(error);}

int main(){
    auto restore=defaults();std::string line;
    while(std::getline(std::cin,line))try {
        auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);
        for(const auto& reset:restore)reset();
        ParamConfigurer::Runtime runtime;
        if(request->containsKey("hashes")) {
            auto hashes=std::get<std::shared_ptr<JSONArray>>(request->get("hashes").value);
            runtime.enumHash=[hashes](size_t index){return uint32_t(std::get<int64_t>(hashes->get(index).value));};
        }
        runtime.language=request->containsKey("language")?request->getString("language"):"en";
        runtime.exit=[](int status){throw Exit{status};};
        ParamConfigurer config(runtime);JSONArray output;
        for(const auto& raw:std::get<std::shared_ptr<JSONArray>>(request->get("ops").value)->elements()) {
            auto op=std::get<std::shared_ptr<JSONObject>>(raw.value);const auto& value=op->get("value");auto name=op->getString("op");JSONObject row;
            std::ostringstream printed;auto prior=std::cout.rdbuf(printed.rdbuf());
            try {
                if(name=="args"||name=="conf"){auto values=args(value);row.put("same",&(name=="args"?config.usingProgramArgs(values?&*values:nullptr):config.usingConfFile(values?&*values:nullptr))==&config);}
                else if(name=="env") {std::optional<ParamConfigurer::Environment> env;if(!value.isNull()){env.emplace();for(const auto& item:std::get<std::shared_ptr<JSONArray>>(value.value)->elements()){auto pair=std::get<std::shared_ptr<JSONArray>>(item.value);env->emplace_back(string(pair->get(0)),string(pair->get(1)));}}row.put("same",&config.usingEnvVariables(env?&*env:nullptr)==&config);}
                else if(name=="configure")row.put("same",&config.configure()==&config);
                else if(name=="checkArgs"){auto values=args(value);row.put("result",ParamConfigurer::checkParams(values?&*values:nullptr));}
                else if(name=="checkText")row.put("result",ParamConfigurer::checkParams(string(value)));
                else if(name=="setTest")setTestParams(op->getString("field"),value);
                else if(name=="setGlobal")setGlobalParams(op->getString("field"),value);
                else if(name=="applyTest")TestParams::setTestParams();
                else if(name=="reset")GeneralParams::resetDefaults();
                else throw std::invalid_argument(name);
            }catch(const std::exception& error){row.put("exception",errorText(error));}catch(const Exit& exit){row.put("exit",exit.status);}
            std::cout.rdbuf(prior);row.put("stdout",printed.str());row.put("general",snapshotGeneralParams());row.put("global",snapshotGlobalParams());row.put("test",snapshotTestParams());row.put("daemon",config.isDaemon());row.put("shutdownhook",config.isShutDownHook());row.put("level",p2p::P2PParams::LOG_LEVEL==p2p::P2PLogLevel::DEBUG?"DEBUG":p2p::P2PParams::LOG_LEVEL==p2p::P2PLogLevel::INFO?"INFO":"NODE_RUNNER_MSG");row.put("params",ParamConfigurerTestAccess::entries(config));output.add(row);
        }
        std::cout<<output.toJSONString()<<std::endl;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';std::cout<<"{\"fatal\":true}"<<std::endl;}
}
