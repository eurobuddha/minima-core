#include "org/minima/utils/sphincs/SPHINCS.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
#include <sstream>
using namespace minima;
using namespace minima::sphincs;
using Object=json::JSONObject;
static std::shared_ptr<MiniData> data(const json::JSONValue& v) {return v.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(v.asString()));}
static Object error(const std::exception& e) {Object r;auto text=nativeExceptionText(e);auto type=text.substr(0,text.find(": "));std::string message=e.what();if(type=="java.lang.NullPointerException"||type=="java.io.EOFException")message.clear();r.put("exception",type);r.put("message",message);return r;}
static Object state(const SPHINCS& signer) {Object r;r.put("public",signer.getPublicKey()?json::JSONValue(signer.getPublicKey()->to0xString()):json::JSONValue());r.put("private",signer.getPrivateKey()?json::JSONValue(signer.getPrivateKey()->to0xString()):json::JSONValue());r.put("total",signer.getTotalWotsKeys()?json::JSONValue(signer.getTotalWotsKeys()->toDecimalString()):json::JSONValue());return r;}
int main(int argc,char** argv) {
    if(argc>1&&std::string(argv[1])=="--demo"){SPHINCS::main();return 0;}
    std::cout<<std::unitbuf;SPHINCS signer;std::string line;
    while(std::getline(std::cin,line)) {
        auto q=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object r;json::JSONArray logs;
        auto oldPub=signer.getPublicKey(),oldPrivate=signer.getPrivateKey();auto oldTotal=signer.getTotalWotsKeys();
        std::ostringstream output;auto previous=std::cout.rdbuf(output.rdbuf());
        try {
            auto action=q->get("action").asString();
            if(action=="fresh")signer=SPHINCS();
            else if(action=="seed")signer.initSeed(data(q->get("seed")).get());
            else if(action=="private"){
                auto priv=data(q->get("private"));try{signer.initPrivateKey(priv);}catch(...){r.put("inputIdentity",priv==signer.getPrivateKey());throw;}r.put("inputIdentity",priv==signer.getPrivateKey());
            }else if(action=="sign"||action=="static"){
                auto message=data(q->get("message"));auto s=action=="static"?SPHINCS::signMessage(message.get(),data(q->get("private"))):signer.signMessage(message.get());
                r.put("wire",MiniData(s.serialise()).to0xString());r.put("json",s.toJSON());
            }else if(action=="verify"){
                std::optional<SPHINCSSignature> s;
                if(auto bytes=data(q->get("wire"))){size_t offset=0;s=SPHINCSSignature::deserialise(bytes->bytes(),offset);int mask=q->containsKey("nulls")?int(std::get<int64_t>(q->get("nulls").value)):0;
                    s=SPHINCSSignature(mask&1?nullptr:s->getWOTSSignature(),mask&2?nullptr:s->getFORSRoot(),mask&4?nullptr:s->getFORSSignature());}
                auto message=data(q->get("message")),pub=data(q->get("public"));r.put("valid",SPHINCS::verifySignature(message.get(),s?&*s:nullptr,pub.get()));
            }else if(action=="log")SPHINCS::log(q->get("message").isNull()?std::nullopt:std::optional<std::string>(q->get("message").asString()));
            else if(action!="state")throw JavaIllegalArgumentException("Unknown fixture action");
        }catch(const std::exception& e){r.put("error",error(e));}
        std::cout.rdbuf(previous);std::istringstream printed(output.str());std::string log;while(std::getline(printed,log)){auto pos=log.find("] : ");logs.add(pos==log.npos?log:log.substr(pos+4));}
        r.put("state",state(signer));r.put("samePublic",oldPub==signer.getPublicKey());r.put("samePrivate",oldPrivate==signer.getPrivateKey());r.put("sameTotal",oldTotal==signer.getTotalWotsKeys());r.put("logs",logs);std::cout<<r.toJSONString()<<'\n';
    }
}
