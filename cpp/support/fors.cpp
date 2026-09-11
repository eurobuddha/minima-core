#include "org/minima/utils/sphincs/FORS/FORS.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;
using namespace minima::sphincs::fors;
using Object=json::JSONObject;
static std::shared_ptr<FORS> forest;
static std::shared_ptr<MiniData> data(const json::JSONValue& value) {return value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString()));}
static int integer(const Object& r,const char* name,int fallback=0) {return r.containsKey(name)?int(std::get<int64_t>(r.get(name).value)):fallback;}
static bool flag(const Object& r,const char* name) {return r.containsKey(name)&&r.get(name).asBool();}
static Object error(const std::exception& e) {
    Object r;auto text=nativeExceptionText(e);auto pos=text.find(": ");auto type=text.substr(0,pos);std::string message=e.what();
    if(type=="java.lang.NullPointerException"||type=="java.io.EOFException")message.clear();r.put("exception",type);r.put("message",message);return r;
}
static std::string hash(const std::vector<uint8_t>& bytes) {return MiniData(bytes).sha3().to0xString();}
static FORSSignature read(const json::JSONValue& value) {auto bytes=data(value);size_t offset=0;return FORSSignature::deserialise(bytes->bytes(),offset);}
static void mutate(FORSSignature& s,const Object& r) {
    auto values=s.getHORSTSignature()->getSignatureValues();auto proofs=s.getHORSTSignature()->getPublicKeyTreeProofs();
    if(r.containsKey("values")&&values->size()>size_t(integer(r,"values")))values->resize(size_t(integer(r,"values")));
    if(r.containsKey("proofs")&&proofs->size()>size_t(integer(r,"proofs")))proofs->resize(size_t(integer(r,"proofs")));
    if(r.containsKey("nullValue"))values->at(size_t(integer(r,"nullValue"))).reset();
    if(r.containsKey("nullProof"))proofs->at(size_t(integer(r,"nullProof"))).reset();
    if(r.containsKey("value"))values->at(size_t(integer(r,"index")))=data(r.get("value"));
    if(r.containsKey("proof")){auto bytes=data(r.get("proof"));size_t offset=0;proofs->at(size_t(integer(r,"index")))=std::make_shared<MMRProof>(MMRProof::deserialise(bytes->bytes(),offset));}
    if(r.containsKey("oversizeProof"))proofs->at(size_t(integer(r,"oversizeProof")))->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
    if(r.containsKey("swap"))std::swap(values->at(0),values->at(size_t(integer(r,"swap"))));
    auto roots=s.getHORSTRoots();auto outer=s.getHORSTTreeProofs();
    if(r.containsKey("roots")&&roots->size()>size_t(integer(r,"roots")))roots->resize(size_t(integer(r,"roots")));
    if(r.containsKey("outerProofs")&&outer->size()>size_t(integer(r,"outerProofs")))outer->resize(size_t(integer(r,"outerProofs")));
    if(r.containsKey("nullRoot"))roots->at(size_t(integer(r,"nullRoot"))).reset();
    if(r.containsKey("nullOuterProof"))outer->at(size_t(integer(r,"nullOuterProof"))).reset();
    if(r.containsKey("rootData"))roots->at(size_t(integer(r,"index")))=std::make_shared<MMRData>(*data(r.get("rootData")),MiniNumber(r.get("rootSum").asString()));
    if(r.containsKey("outerProof")){auto bytes=data(r.get("outerProof"));size_t offset=0;outer->at(size_t(integer(r,"index")))=std::make_shared<MMRProof>(MMRProof::deserialise(bytes->bytes(),offset));}
    if(r.containsKey("oversizeOuterProof"))outer->at(size_t(integer(r,"oversizeOuterProof")))->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
    if(flag(r,"extra")){values->push_back(nullptr);proofs->push_back(nullptr);roots->push_back(nullptr);outer->push_back(nullptr);}
}
static Object snapshot(const FORSSignature& s) {
    Object r;r.put("values",int64_t(s.getHORSTSignature()->getSignatureValues()->size()));r.put("proofs",int64_t(s.getHORSTSignature()->getPublicKeyTreeProofs()->size()));r.put("roots",int64_t(s.getHORSTRoots()->size()));r.put("outerProofs",int64_t(s.getHORSTTreeProofs()->size()));
    std::vector<uint8_t> bytes;try{s.writeDataStream(bytes);}catch(const std::exception& e){r.put("writeError",error(e));}
    r.put("wireHash",hash(bytes));r.put("wireLength",int64_t(bytes.size()));try{r.put("json",s.toJSON());}catch(const std::exception& e){r.put("jsonError",error(e));}return r;
}
int main(int argc,char** argv) {
    if(argc>1&&std::string(argv[1])=="--demo"){FORS::main();return 0;}
    std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line)) {
        auto request=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object result;int logs=0;json::JSONArray kinds;
        MinimaLogger::Runtime runtime;runtime.println=[&](const auto& value){auto text=unicode::encodeUTF8(value);auto pos=text.find("] : java.");if(pos!=text.npos){++logs;if(kinds.size()<2){auto type=text.substr(pos+4);kinds.add(type.substr(0,type.find(": ")));}}};MinimaLogger::setRuntime(std::move(runtime));
        try {
            auto action=request->get("action").asString();
            if(action=="init") {
                auto seed=data(request->get("seed"));forest=std::make_shared<FORS>(seed.get());result.put("root",forest->getForsRoot()->toJSON());result.put("rootIdentity",forest->getForsRoot()==forest->getForsRoot());
            }else if(action=="sign") {
                auto message=data(request->get("message"));auto s=forest->signMessage(message.get());result.put("wire",MiniData(s.serialise()).to0xString());result.put("json",s.toJSON());result.put("root",forest->getForsRoot()->toJSON());
            }else if(action=="verify") {
                std::optional<FORSSignature> s;if(!request->get("wire").isNull()){s=read(request->get("wire"));mutate(*s,*request);}
                std::optional<MMRData> root;if(!request->get("root").isNull())root.emplace(*data(request->get("root")),MiniNumber(request->get("sum").asString()));
                auto message=data(request->get("message"));result.put("valid",FORS::verifySignature(message.get(),s?&*s:nullptr,root?&*root:nullptr));
            }else if(action=="read") {
                auto s=read(request->get("initial"));auto oldSig=s.getHORSTSignature();auto oldRoots=s.getHORSTRoots();auto oldProofs=s.getHORSTTreeProofs();auto bytes=data(request->get("wire"));size_t offset=0;
                try{s.readDataStream(bytes->bytes(),offset);}catch(const std::exception& e){result.put("readError",error(e));}
                result.put("position",int64_t(offset));result.put("sameSignature",oldSig==s.getHORSTSignature());result.put("sameRoots",oldRoots==s.getHORSTRoots());result.put("sameProofs",oldProofs==s.getHORSTTreeProofs());
                result.put("oldValues",int64_t(oldSig->getSignatureValues()->size()));result.put("oldRoots",int64_t(oldRoots->size()));result.put("oldProofs",int64_t(oldProofs->size()));result.put("state",snapshot(s));
            }else if(action=="convert") {auto bytes=data(request->get("wire"));result.put("state",snapshot(FORSSignature::convertMiniDataVersion(bytes.get())));}
            else if(action=="mutate"){auto s=read(request->get("wire"));mutate(s,*request);result.put("state",snapshot(s));}
            else if(action=="alias") {
                auto message=data(request->get("message"));auto a=forest->signMessage(message.get()),b=forest->signMessage(message.get());bool roots=true,secrets=true,proofs=true;
                for(int i=0;i<16;++i){roots&=a.getHORSTRoots()->at(i)==b.getHORSTRoots()->at(i);secrets&=a.getHORSTSignature()->getSignatureValues()->at(i)==b.getHORSTSignature()->getSignatureValues()->at(i);proofs&=a.getHORSTTreeProofs()->at(i)!=b.getHORSTTreeProofs()->at(i);}
                result.put("sharedRoots",roots);result.put("sharedSecrets",secrets);result.put("distinctProofs",proofs);result.put("distinctLists",a.getHORSTRoots()!=b.getHORSTRoots());
                auto list=a.getHORSTRoots();list->erase(list->begin());result.put("aliasSize",int64_t(a.getHORSTRoots()->size()));result.put("otherSize",int64_t(b.getHORSTRoots()->size()));
            }else throw JavaIllegalArgumentException("unknown fixture action");
        }catch(const std::exception& e){result.put("error",error(e));}
        MinimaLogger::setRuntime({});result.put("logCount",logs);result.put("logKinds",kinds);std::cout<<result.toJSONString()<<'\n';
    }
}
