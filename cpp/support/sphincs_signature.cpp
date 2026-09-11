#include "org/minima/utils/sphincs/SPHINCSSignature.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;
using namespace minima::sphincs;
using Object=json::JSONObject;
static Object error(const std::exception& e) {Object r;auto text=nativeExceptionText(e);auto type=text.substr(0,text.find(": "));std::string message=e.what();if(type=="java.lang.NullPointerException"||type=="java.io.EOFException")message.clear();r.put("exception",type);r.put("message",message);return r;}
static std::shared_ptr<MiniData> data(const json::JSONValue& v) {return v.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(v.asString()));}
static bool flag(const Object& q,const char* key) {return q.containsKey(key)&&q.get(key).asBool();}
static Object snapshot(const SPHINCSSignature& s) {
    Object r;auto w=s.getWOTSSignature();auto root=s.getFORSRoot();auto f=s.getFORSSignature();r.put("wotsNull",!w);r.put("rootNull",!root);r.put("forsNull",!f);
    if(w)r.put("wotsCount",int64_t(w->getAllSignatureProofs()->size()));if(root)r.put("root",root->toJSON());
    if(f){r.put("values",int64_t(f->getHORSTSignature()->getSignatureValues()->size()));r.put("proofs",int64_t(f->getHORSTSignature()->getPublicKeyTreeProofs()->size()));r.put("roots",int64_t(f->getHORSTRoots()->size()));r.put("outerProofs",int64_t(f->getHORSTTreeProofs()->size()));}
    std::vector<uint8_t> bytes;try{s.writeDataStream(bytes);}catch(const std::exception& e){r.put("writeError",error(e));}
    r.put("wireHash",MiniData(bytes).sha3().to0xString());r.put("wireLength",int64_t(bytes.size()));try{r.put("json",s.toJSON());}catch(const std::exception& e){r.put("jsonError",error(e));}return r;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){
    auto q=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object r;int logs=0;json::JSONArray kinds;
    MinimaLogger::Runtime runtime;runtime.println=[&](const auto& v){auto text=unicode::encodeUTF8(v);auto pos=text.find("] : java.");if(pos!=text.npos){++logs;if(kinds.size()<2){auto type=text.substr(pos+4);kinds.add(type.substr(0,type.find(": ")));}}};MinimaLogger::setRuntime(std::move(runtime));
    try{
        auto action=q->get("action").asString();
        if(action=="convert"){auto bytes=data(q->get("wire"));r.put("state",snapshot(SPHINCSSignature::convertMiniDataVersion(bytes.get())));}
        else {
            std::shared_ptr<Signature> wots;std::shared_ptr<MMRData> root;std::shared_ptr<fors::FORSSignature> f;
            if(auto bytes=data(q->get("wots"))){size_t offset=0;wots=std::make_shared<Signature>(Signature::deserialise(bytes->bytes(),offset));}
            if(auto bytes=data(q->get("root")))root=std::make_shared<MMRData>(*bytes,MiniNumber(q->get("sum").asString()));
            if(auto bytes=data(q->get("fors"))){size_t offset=0;f=std::make_shared<fors::FORSSignature>(fors::FORSSignature::deserialise(bytes->bytes(),offset));}
            auto s=flag(*q,"default")?SPHINCSSignature():SPHINCSSignature(wots,root,f);auto oldWots=s.getWOTSSignature();auto oldRoot=s.getFORSRoot();auto oldFors=s.getFORSSignature();
            if(action=="read"){
                auto bytes=data(q->get("wire"));size_t offset=0;try{s.readDataStream(bytes->bytes(),offset);}catch(const std::exception& e){r.put("readError",error(e));}
                r.put("position",int64_t(offset));r.put("sameWots",oldWots==s.getWOTSSignature());r.put("sameRoot",oldRoot==s.getFORSRoot());r.put("sameFors",oldFors==s.getFORSSignature());
                r.put("oldWotsCount",oldWots?int64_t(oldWots->getAllSignatureProofs()->size()):-1);r.put("oldRoots",oldFors?int64_t(oldFors->getHORSTRoots()->size()):-1);
            }else{
                r.put("wotsIdentity",wots==s.getWOTSSignature());r.put("rootIdentity",root==s.getFORSRoot());r.put("forsIdentity",f==s.getFORSSignature());
                if(flag(*q,"addWots")){if(!wots)throw JavaNullPointerException("");wots->addSignatureProof(SignatureProof(MiniData::fromHex("0xAA"),MiniData::fromHex("0xBB"),MMRProof()));}
                if(flag(*q,"clearForsRoots")){if(!f)throw JavaNullPointerException("");f->getHORSTRoots()->clear();}
                if(flag(*q,"badWotsProof")){auto proof=wots->getAllSignatureProofs()->at(0).getProof();proof.addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));}
                if(q->containsKey("forsMutation")){
                    auto mutation=std::get<std::shared_ptr<Object>>(q->get("forsMutation").value);
                    if(mutation->containsKey("rootData"))f->getHORSTRoots()->at(0)=std::make_shared<MMRData>(*data(mutation->get("rootData")),MiniNumber(mutation->get("rootSum").asString()));
                    if(mutation->containsKey("oversizeOuterProof"))f->getHORSTTreeProofs()->at(0)->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
                }
            }
            r.put("state",snapshot(s));
        }
    }catch(const std::exception& e){r.put("error",error(e));}
    MinimaLogger::setRuntime({});r.put("logCount",logs);r.put("logKinds",kinds);std::cout<<r.toJSONString()<<'\n';
}}
