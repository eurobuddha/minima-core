#include "org/minima/system/brains/TxPoWChecker.hpp"
#include "org/minima/system/params/GeneralParams.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/parser/JSONParser.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
#include <sstream>
using namespace minima;using namespace minima::json;
static bool flag(const JSONObject& in,const char* name){return in.containsKey(name)&&in.get(name).asBool();}
static std::string flags(const TxPoW& tx){auto str=[](bool b){return b?"true":"false";};auto& a=tx.getTransaction();auto& b=tx.getBurnTransaction();return std::string(str(a.mHaveCheckedMonotonic()))+":"+str(a.mIsMonotonic())+":"+str(a.mIsValid())+":"+str(b.mHaveCheckedMonotonic())+":"+str(b.mIsMonotonic())+":"+str(b.mIsValid());}
template<class T> T read(std::istream& in){std::string text;in>>text;auto data=MiniData::fromHex(text);size_t offset=0;return T::deserialise(data.bytes(),offset);}
static JSONObject run(const JSONObject& input){
    std::istringstream stream(input.get("wire").asString());auto mmr=read<MMR>(stream);auto tx=read<TxPoW>(stream),block=read<TxPoW>(stream);auto method=input.get("method").asString(),kind=input.get("kind").asString();int failAt=int(std::get<int64_t>(input.get("failAt").value)),prints=0;JSONArray logs;
    GeneralParams::SCRIPTLOGS=flag(input,"trace");if(flag(input,"badMMR"))mmr=MMR();if(input.containsKey("maxSize"))block.getMagic().mCurrentMaxTxPoWSize()=std::make_shared<MiniNumber>(MiniNumber(input.get("maxSize").asString()));
    MinimaLogger::Runtime logger;logger.println=[&](const auto& full){auto text=unicode::encodeUTF8(full);auto at=text.find("] : ");if(at==std::string::npos)return;auto body=text.substr(at+4);if(body.starts_with("     "))return;logs.add(body);if(++prints==failAt){if(kind=="error")throw JavaError("java.lang.LinkageError","logger");if(kind=="oom")throw JavaOutOfMemoryError("logger");throw JavaTypedException("java.lang.IllegalStateException","logger");}};MinimaLogger::setRuntime(logger);struct Reset{~Reset(){MinimaLogger::setRuntime({});}}reset;
    JSONValue result;const auto* tip=flag(input,"nullMMR")?nullptr:&mmr;
    try{if(method=="basic")result=TxPoWChecker::checkTxPoWBasic(tx);else if(method=="simple")result=TxPoWChecker::checkTxPoWSimple(tip,tx,block,flag(input,"log"));else if(method=="scripts")result=TxPoWChecker::checkTxPoWScripts(tip,tx,block);else if(method=="mmr")result=TxPoWChecker::checkMMR(tip,tx,flag(input,"log"));else if(method=="signatures")result=TxPoWChecker::checkSignatures(tx);else throw std::invalid_argument(method);}catch(const std::exception& e){result="escaped:"+nativeExceptionText(e);}
    JSONObject out;out.put("result",result);out.put("logs",logs);try{out.put("flags",flags(tx));out.put("txid",tx.getTransaction().transactionIDValue().to0xString());}catch(const std::exception& e){auto text=nativeExceptionText(e);out.put("snapshotError",text.substr(0,text.find(':')));}return out;
}
int main(){std::string line;while(std::getline(std::cin,line))try{auto parsed=parser::JSONParser().parse(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(parsed.value)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
