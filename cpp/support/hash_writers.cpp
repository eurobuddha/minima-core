#include "org/minima/objects/TxBody.hpp"
#include "org/minima/objects/Magic.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static int integer(const JSONObject& q,const std::string& key,int fallback){return q.containsKey(key)?int(std::get<int64_t>(q.get(key).value)):fallback;}
struct Limits{int hash=MiniData::MINIMA_MAX_HASH_LENGTH,data=MiniData::MINIMA_MAX_MINIDATA_LENGTH;~Limits(){MiniData::MINIMA_MAX_HASH_LENGTH=hash;MiniData::MINIMA_MAX_MINIDATA_LENGTH=data;}};
class Ordinary:public MiniData {
public:
    std::string mode;std::shared_ptr<JSONArray> trace;std::function<void()> action;
    Ordinary(std::vector<uint8_t> bytes,std::string m,std::shared_ptr<JSONArray> t):MiniData(std::move(bytes)),mode(std::move(m)),trace(std::move(t)){}
    void writeDataStream(std::vector<uint8_t>& out)const override{
        trace->add("ordinary");out.push_back(0xA1);if(action)action();
        if(mode.starts_with("super"))MiniData::writeDataStream(out);
        trace->add("ordinary-after");if(mode.ends_with("io"))throw wire::IOException("ordinary fixture");
    }
};
class Hash:public Ordinary {
public:
    using Ordinary::Ordinary;
    void writeHashToStream(std::vector<uint8_t>& out)const override{
        trace->add("hash");out.push_back(0xB2);if(action)action();
        if(mode.starts_with("super"))MiniData::writeHashToStream(out);
        trace->add("hash-after");if(mode.ends_with("io"))throw wire::IOException("hash fixture");
    }
};
static JSONObject run(const JSONObject& q){
    Limits restore;MiniData::MINIMA_MAX_HASH_LENGTH=integer(q,"hashLimit",64);MiniData::MINIMA_MAX_MINIDATA_LENGTH=integer(q,"dataLimit",512*1024*1024);
    auto bytes=std::vector<uint8_t>(size_t(integer(q,"length",1)),0x44);auto trace=std::make_shared<JSONArray>();auto kind=q.getString("kind"),mode=q.getString("mode"),context=q.getString("context"),method=q.getString("method");
    std::shared_ptr<MiniData> child;if(kind=="ordinary")child=std::make_shared<Ordinary>(bytes,mode,trace);else if(kind=="hash")child=std::make_shared<Hash>(bytes,mode,trace);else child=std::make_shared<MiniData>(bytes);
    std::shared_ptr<Streamable> owner;std::function<std::string()> current;
    if(context=="transaction"){
        auto txn=std::make_shared<Transaction>();txn->setLinkHash(child);owner=txn;current=[txn]{return txn->getLinkHash()->to0xString();};
        if(auto derived=std::dynamic_pointer_cast<Ordinary>(child);derived&&q.getBoolean("replace")){std::weak_ptr<Transaction> weak=txn;derived->action=[weak]{if(auto txn=weak.lock())txn->setLinkHash(MiniData::fromHex("0x99"));};}
    }else if(context=="prng"||context=="id"){
        auto body=std::make_shared<TxBody>();body->mPRNG()=std::make_shared<MiniData>(MiniData::fromHex("0x11"));body->mTxnDifficulty()=std::make_shared<MiniData>(MiniData::fromHex("0x22"));owner=body;
        if(context=="prng")body->mPRNG()=child;else{body->mTxPowIDList()->add(std::make_shared<MiniData>(MiniData::fromHex("0x33")));body->mTxPowIDList()->add(child);}
        current=[body,context]{return (context=="prng"?body->mPRNG():body->mTxPowIDList()->get(1))->to0xString();};
        if(auto derived=std::dynamic_pointer_cast<Ordinary>(child);derived&&q.getBoolean("replace")){std::weak_ptr<TxBody> weak=body;derived->action=[weak,context]{if(auto body=weak.lock()){auto replacement=std::make_shared<MiniData>(MiniData::fromHex("0x99"));if(context=="prng")body->mPRNG()=replacement;else body->mTxPowIDList()->set(1,replacement);}};}
    }else if(context=="magic"){
        auto magic=std::make_shared<Magic>();magic->mCurrentMinTxPoWWork()=child;owner=magic;current=[magic]{return magic->mCurrentMinTxPoWWork()->to0xString();};
        if(auto derived=std::dynamic_pointer_cast<Ordinary>(child);derived&&q.getBoolean("replace")){std::weak_ptr<Magic> weak=magic;derived->action=[weak]{if(auto magic=weak.lock())magic->mCurrentMinTxPoWWork()=std::make_shared<MiniData>(MiniData::fromHex("0x99"));};}
    }else owner=child;
    // The containing object is the only strong owner of nested children at dispatch.
    child.reset();JSONObject result;std::vector<uint8_t> output{0xAA,0x55};
    try{
        if(method=="read"||method=="readHash"){
            auto input=MiniData::fromHex(q.getString("wire")).bytes();size_t offset=0;auto& data=dynamic_cast<MiniData&>(*owner);
            try{if(method=="readHash")data.readHashFromStream(input,offset);else data.readDataStream(input,offset);}catch(const std::exception& e){result.put("error",nativeExceptionText(e));}
            result.put("offset",int64_t(offset));result.put("value",data.to0xString());
        }else if(context!="direct")owner->writeDataStream(output);
        else if(method=="hash")dynamic_cast<MiniData&>(*owner).writeHashToStream(output);
        else if(method=="hashSerial")wire::append(output,dynamic_cast<MiniData&>(*owner).serialiseHash());
        else if(method=="serial")wire::append(output,owner->serialise());
        else owner->writeDataStream(output);
    }catch(const std::exception& e){result.put("error",nativeExceptionText(e));}
    result.put("wire",MiniData(output).to0xString());result.put("trace",*trace);if(current)result.put("current",current());return result;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto parsed=JSONValue::parseWithException(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(parsed.value)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
