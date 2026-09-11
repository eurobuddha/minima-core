#include "org/minima/utils/ZipExtractor.hpp"
#include "org/minima/utils/MinimaLogger.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/JavaUnixFile.hpp"
#include "support/JavaExceptions.hpp"
#include "support/ByteIO.hpp"
#include "support/JavaUnicode.hpp"
#include <iostream>
using namespace minima;
namespace minima {
class ZipExtractorFixture {
public:
    static auto path(const std::optional<std::filesystem::path>& destination,const std::string& name) { return ZipExtractor::newFile(destination,name); }
};
}
int main() {
    std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line)) {
        auto request=std::get<std::shared_ptr<json::JSONObject>>(json::JSONValue::parseWithException(line).value);
        json::JSONObject result;json::JSONArray calls,logs;
        size_t position=0;int closes=0;
        auto number=[&](const char* key,int fallback){return request->containsKey(key)?int(std::get<int64_t>(request->get(key).value)):fallback;};
        auto flag=[&](const char* key){return request->containsKey(key)&&request->get(key).asBool();};
        auto bytes=MiniData::fromHex(request->get("bytes").asString()).bytes();
        std::optional<std::filesystem::path> destination;
        if(!request->get("destination").isNull())destination=*javaUnixFile(std::filesystem::path(request->get("destination").asString()));
        MinimaLogger::Runtime runtime;runtime.println=[&](const auto& value){auto s=unicode::encodeUTF8(value);auto marker=s.find("] : ");logs.add(marker==s.npos?s:s.substr(marker+4));};
        MinimaLogger::setRuntime(std::move(runtime));
        try {
            if(flag("helper")) {
                if(request->get("name").isNull())throw JavaNullPointerException("");
                auto name=request->get("name").asString();
                if(request->get("name").asUTF16().size()+46>65535)throw JavaIllegalArgumentException("entry name too long");
                result.put("path",ZipExtractorFixture::path(destination,name).string());
            } else {
                ZipExtractor::Reader reader;
                if(!flag("nullInput"))reader=[&](std::span<uint8_t> output) {
                    json::JSONArray event;event.add(int64_t(output.size()));
                    int fail=number("failAt",-1);
                    if(fail>=0 && position>=size_t(fail)) {event.add("throw");calls.add(event);throw wire::IOException("fixture read failure");}
                    size_t count=std::min({output.size(),bytes.size()-position,size_t(number("chunk",4096))});
                    if(fail>=0)count=std::min(count,size_t(fail)-position);
                    std::copy_n(bytes.begin()+position,count,output.begin());position+=count;
                    event.add(count?int64_t(count):int64_t(-1));calls.add(event);return count;
                };
                ZipExtractor::unzip(std::move(reader),destination,[&]{++closes;if(flag("failClose"))throw wire::IOException("fixture close failure");});
                result.put("success",true);
            }
        } catch(const std::exception& error) {
            std::string type,message=error.what();
            if(auto e=dynamic_cast<const wire::TypedIOException*>(&error))type=e->className();
            else if(dynamic_cast<const wire::EOFException*>(&error))type="java.io.EOFException";
            else if(dynamic_cast<const wire::IOException*>(&error))type="java.io.IOException";
            else {auto full=javaExceptionText(error);auto marker=full.find(": ");type=full.substr(0,marker);}
            if(type=="java.lang.NullPointerException")message.clear();
            result.put("exception",type);result.put("message",message);
        }
        MinimaLogger::setRuntime({});
        result.put("position",int64_t(position));result.put("closes",closes);result.put("reads",calls);result.put("logs",logs);
        std::cout<<result.toJSONString()<<'\n';
    }
}
