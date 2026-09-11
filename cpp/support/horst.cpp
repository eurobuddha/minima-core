#include "org/minima/utils/sphincs/HORST/HORST.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;
using namespace minima::sphincs::horst;
using Object=json::JSONObject;
static std::shared_ptr<HORST> key;
static std::shared_ptr<MiniData> data(const json::JSONValue& value) {return value.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(value.asString()));}
static int integer(const Object& r,const char* name,int fallback=0) {return r.containsKey(name)?int(std::get<int64_t>(r.get(name).value)):fallback;}
static bool flag(const Object& r,const char* name) {return r.containsKey(name)&&r.get(name).asBool();}
static Object error(const std::exception& e) {
    Object r;auto text=nativeExceptionText(e);auto pos=text.find(": ");auto type=text.substr(0,pos);std::string message=e.what();
    if(type=="java.lang.NullPointerException"||type=="java.io.EOFException")message.clear();r.put("exception",type);r.put("message",message);return r;
}
static std::string hash(const std::vector<uint8_t>& bytes) {return MiniData(bytes).sha3().to0xString();}
static HORSTSignature read(const json::JSONValue& value) {auto bytes=data(value);size_t offset=0;return HORSTSignature::deserialise(bytes->bytes(),offset);}
static void mutate(HORSTSignature& s,const Object& r) {
    auto values=s.getSignatureValues();auto proofs=s.getPublicKeyTreeProofs();
    if(r.containsKey("values")&&values->size()>size_t(integer(r,"values")))values->resize(size_t(integer(r,"values")));
    if(r.containsKey("proofs")&&proofs->size()>size_t(integer(r,"proofs")))proofs->resize(size_t(integer(r,"proofs")));
    if(r.containsKey("nullValue"))values->at(size_t(integer(r,"nullValue"))).reset();
    if(r.containsKey("nullProof"))proofs->at(size_t(integer(r,"nullProof"))).reset();
    if(r.containsKey("value"))values->at(size_t(integer(r,"index")))=data(r.get("value"));
    if(r.containsKey("proof")){auto bytes=data(r.get("proof"));size_t offset=0;proofs->at(size_t(integer(r,"index")))=std::make_shared<MMRProof>(MMRProof::deserialise(bytes->bytes(),offset));}
    if(r.containsKey("oversizeProof"))proofs->at(size_t(integer(r,"oversizeProof")))->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
    if(r.containsKey("swap"))std::swap(values->at(0),values->at(size_t(integer(r,"swap"))));
    if(flag(r,"extra")){values->push_back(nullptr);proofs->push_back(nullptr);}
}
static Object snapshot(const HORSTSignature& s) {
    Object result;result.put("values",int64_t(s.getSignatureValues()->size()));result.put("proofs",int64_t(s.getPublicKeyTreeProofs()->size()));
    std::vector<uint8_t> bytes;try{s.writeDataStream(bytes);}catch(const std::exception& e){result.put("writeError",error(e));}
    result.put("wireHash",hash(bytes));result.put("wireLength",int64_t(bytes.size()));
    try{result.put("json",s.toJSON());}catch(const std::exception& e){result.put("jsonError",error(e));}return result;
}
int main(int argc,char** argv) {
    if(argc>1&&std::string(argv[1])=="--demo"){HORST::main();return 0;}
    std::cout<<std::unitbuf;std::string line;
    while(std::getline(std::cin,line)) {
        auto request=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object result;int logs=0;json::JSONArray kinds;
        MinimaLogger::Runtime runtime;runtime.println=[&](const auto& value){auto text=unicode::encodeUTF8(value);auto pos=text.find("] : java.");if(pos!=text.npos){++logs;if(kinds.size()<2){auto type=text.substr(pos+4);kinds.add(type.substr(0,type.find(": ")));}}};MinimaLogger::setRuntime(std::move(runtime));
        try {
            auto action=request->get("action").asString();
            if(action=="init") {
                auto seed=data(request->get("seed"));key=std::make_shared<HORST>(seed.get());result.put("root",key->getHORSTRoot()->toJSON());
                result.put("privateSize",key->getPrivateKey()->getSize());result.put("publicSize",key->getPublicKey()->getSize());std::vector<uint8_t> priv,pub;
                for(int i=0;i<key->getPrivateKey()->getSize();++i){wire::append(priv,key->getPrivateKey()->getKey(i)->bytes());wire::append(pub,key->getPublicKey()->getKey(i)->bytes());}
                result.put("privateHash",hash(priv));result.put("publicHash",hash(pub));result.put("rootIdentity",key->getHORSTRoot()==key->getPublicKey()->getPublicKeyTreeRoot());
            } else if(action=="key") {
                auto index=integer(*request,"index");auto kind=request->get("kind").asString();
                if(kind=="private"){result.put("data",key->getPrivateKey()->getKey(index)->to0xString());result.put("identity",key->getPrivateKey()->getKey(index)==key->getPrivateKey()->getKey(index));}
                else if(kind=="public")result.put("data",key->getPublicKey()->getKey(index)->to0xString());
                else result.put("wire",MiniData(key->getPublicKey()->getKeyTreeProof(index).serialise()).to0xString());
            } else if(action=="ref") {auto original=data(request->get("data"));result.put("ref",HORST::getKeyRef(integer(*request,"index"),original.get()));}
            else if(action=="sign") {
                auto message=data(request->get("message"));auto s=key->signMessage(message.get());result.put("wire",MiniData(s.serialise()).to0xString());result.put("root",key->getHORSTRoot()->toJSON());result.put("json",s.toJSON());
                MiniData hm(Crypto::getInstance().hashData(message->bytes()));bool identity=true;for(int i=0;i<16;++i)identity&=s.getSignatureValues()->at(i)==key->getPrivateKey()->getKey(HORST::getKeyRef(i,hm));result.put("identity",identity);
            } else if(action=="verify") {
                std::optional<HORSTSignature> s;if(!request->get("wire").isNull()){s=read(request->get("wire"));mutate(*s,*request);}
                std::optional<MMRData> root;if(!request->get("root").isNull())root.emplace(*data(request->get("root")),MiniNumber(request->get("sum").asString()));
                auto message=data(request->get("message"));result.put("valid",HORST::verifySignature(message.get(),s?&*s:nullptr,root?&*root:nullptr));
            } else if(action=="read") {
                auto s=read(request->get("initial"));auto oldValues=s.getSignatureValues();auto oldProofs=s.getPublicKeyTreeProofs();auto bytes=data(request->get("wire"));size_t offset=0;
                try{s.readDataStream(bytes->bytes(),offset);}catch(const std::exception& e){result.put("readError",error(e));}
                result.put("position",int64_t(offset));result.put("oldValues",int64_t(oldValues->size()));result.put("oldProofs",int64_t(oldProofs->size()));
                result.put("sameValues",oldValues==s.getSignatureValues());result.put("sameProofs",oldProofs==s.getPublicKeyTreeProofs());result.put("state",snapshot(s));
            } else if(action=="convert") {auto bytes=data(request->get("wire"));result.put("state",snapshot(HORSTSignature::convertMiniDataVersion(bytes.get())));}
            else if(action=="mutate"){auto s=read(request->get("wire"));mutate(s,*request);result.put("state",snapshot(s));}
            else if(action=="alias") {
                auto message=data(request->get("message"));auto s=key->signMessage(message.get());auto values=s.getSignatureValues();auto proofs=s.getPublicKeyTreeProofs();auto proof=proofs->at(0);
                proof->addProofChunk(false,MMRData(MiniData::fromHex("0x1122"),MiniNumber(3)));result.put("proofIdentity",proofs->at(0)==proof);result.put("proofLength",s.getPublicKeyTreeProofs()->at(0)->getProofLength());
                values->erase(values->begin());proofs->erase(proofs->begin());result.put("state",snapshot(s));
            } else if(action=="publicNull"){PublicKey value(nullptr);}
            else if(action=="privateNull"){PrivateKey p(nullptr);result.put("size",p.getSize());result.put("firstNull",!p.getKey(0));result.put("lastNull",!p.getKey(65535));}
            else throw JavaIllegalArgumentException("unknown fixture action");
        }catch(const std::exception& e){result.put("error",error(e));}
        MinimaLogger::setRuntime({});result.put("logCount",logs);result.put("logKinds",kinds);std::cout<<result.toJSONString()<<'\n';
    }
}
