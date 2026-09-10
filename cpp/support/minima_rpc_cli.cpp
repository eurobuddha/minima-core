#include "org/minima/utils/MinimaRPCClient.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include "support/SocketCharacterReader.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include <sstream>
#include <iostream>
using namespace minima;
using namespace minima::json;
class Input final:public MinimaRPCClient::Input {
    wire::Bytes mBytes;size_t mOffset=0;bool mClosed=false,mFail,mReadFail;
    JSONArray& mEvents;
    SocketCharacterReader mReader;
public:
    Input(wire::Bytes bytes,bool fail,bool readFail,JSONArray& events):mBytes(std::move(bytes)),mFail(fail),mReadFail(readFail),mEvents(events),mReader([this](std::span<uint8_t> bytes)->int32_t {
        if(mReadFail){mReadFail=false;throw wire::IOException("fixture read");}
        auto size=std::min(bytes.size(),mBytes.size()-mOffset);if(!size)return -1;
        std::copy_n(mBytes.begin()+mOffset,size,bytes.begin());mOffset+=size;return static_cast<int32_t>(size);
    }){}
    MinimaRPCClient::String readLine() override{return mReader.readLineUTF16(true);}
    void closeBufferedReader() override {mClosed=true;mEvents.add("close");if(mFail)throw wire::IOException("fixture close");}
    void closeInputStreamReader() override {if(!mClosed)closeBufferedReader();}
};
int main() {
    std::string line;
    while(std::getline(std::cin,line)) {
        auto request=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);
        std::vector<MinimaRPCClient::String> args;
        bool nullArgs=request->get("args").isNull();
        if(!nullArgs)for(const auto& value:std::get<std::shared_ptr<JSONArray>>(request->get("args").value)->elements())
            args.push_back(value.isNull()?std::nullopt:std::optional(value.asUTF16()));
        JSONObject output;JSONArray events;std::ostringstream out,err;
        auto previousOut=std::cout.rdbuf(out.rdbuf());auto previousErr=std::cerr.rdbuf(err.rdbuf());
        MinimaRPCClient::Runtime runtime;
        runtime.openInput=[&](){return std::make_shared<Input>(BaseConverter::decode16(request->containsKey("input")?request->getString("input"):"657869740a"),request->containsKey("closeFail")&&request->get("closeFail").asBool(),request->containsKey("readFail")&&request->get("readFail").asBool(),events);};
        try {output.put("status",MinimaRPCClient::main(nullArgs?nullptr:&args,runtime));}
        catch(const std::exception& error){output.put("exception",nativeExceptionText(error));}
        std::cout.rdbuf(previousOut);std::cerr.rdbuf(previousErr);
        output.put("stdout",out.str());output.put("stderr",err.str());output.put("events",events);
        std::cout<<output.toJSONString()<<std::endl;
    }
}
