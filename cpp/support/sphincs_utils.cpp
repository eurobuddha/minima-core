#include "CoinFixture.hpp"
#include "org/minima/utils/sphincs/SPHINCSUtils.hpp"
#include "org/minima/utils/sphincs/SPHINCS.hpp"
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
#include <iostream>
using namespace minima;using namespace minima::sphincs;using Object=json::JSONObject;
static std::shared_ptr<MiniData> data(const json::JSONValue& v){return v.isNull()?nullptr:std::make_shared<MiniData>(MiniData::fromHex(v.asString()));}
static bool flag(const Object& q,const char* k){return q.containsKey(k)&&q.get(k).asBool();}
static int integer(const Object& q,const char* k,int fallback=0){return q.containsKey(k)?int(std::get<int64_t>(q.get(k).value)):fallback;}
static Object error(const std::exception& e){Object r;auto text=nativeExceptionText(e);auto type=text.substr(0,text.find(": "));r.put("exception",type);r.put("message",type=="java.lang.NullPointerException"||type=="java.io.EOFException"?"":e.what());return r;}
static std::shared_ptr<Transaction> transaction(const Object& q){
    if(flag(q,"nullTransaction"))return nullptr;auto t=std::make_shared<Transaction>();
    for(auto key:{"inputs","outputs"})for(const auto& n:std::get<std::shared_ptr<json::JSONArray>>(q.get(key).value)->elements()){auto c=support::coinFixture(int(std::get<int64_t>(n.value)));if(std::string(key)=="inputs")t->addInput(c);else t->addOutput(c);}
    if(flag(q,"eltoo")&&!t->getAllInputs().empty())t->getAllInputs()[0].resetCoinID(Coin::COINID_ELTOO);
    if(flag(q,"oldState")){for(int i=0;i<80;++i)t->addStateVariable(StateVariable(i,"[old]"));t->addStateVariable(StateVariable(100,"[old]"));t->addStateVariable(StateVariable(101,"[old]"));t->addStateVariable(StateVariable(200,"[keep]"));}return t;
}
static Object snapshot(const std::shared_ptr<Transaction>& t){Object r;if(!t){r.put("null",true);return r;}r.put("cachedID",t->getTransactionID().to0xString());json::JSONArray states,ids;for(const auto& s:t->getCompleteState())states.add(s.toJSON());for(const auto& c:t->getAllOutputs())ids.add(c.getCoinID().to0xString());r.put("state",states);r.put("outputIDs",ids);r.put("wire",MiniData(t->serialise()).to0xString());return r;}
static void mutate(fors::FORSSignature& s,const Object& q){
    auto h=s.getHORSTSignature();
    for(auto key:{"roots","outerProofs","values","proofs"})if(q.containsKey(key)){size_t n=size_t(integer(q,key));if(std::string(key)=="roots")s.getHORSTRoots()->resize(std::min(n,s.getHORSTRoots()->size()));else if(std::string(key)=="outerProofs")s.getHORSTTreeProofs()->resize(std::min(n,s.getHORSTTreeProofs()->size()));else if(std::string(key)=="values")h->getSignatureValues()->resize(std::min(n,h->getSignatureValues()->size()));else h->getPublicKeyTreeProofs()->resize(std::min(n,h->getPublicKeyTreeProofs()->size()));}
    if(q.containsKey("nullRoot"))s.getHORSTRoots()->at(integer(q,"nullRoot"))=nullptr;
    if(q.containsKey("nullOuterProof"))s.getHORSTTreeProofs()->at(integer(q,"nullOuterProof"))=nullptr;
    if(q.containsKey("nullValue"))h->getSignatureValues()->at(integer(q,"nullValue"))=nullptr;
    if(q.containsKey("nullProof"))h->getPublicKeyTreeProofs()->at(integer(q,"nullProof"))=nullptr;
    if(q.containsKey("oversizeOuterProof"))s.getHORSTTreeProofs()->at(integer(q,"oversizeOuterProof"))->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
    if(q.containsKey("oversizeProof"))h->getPublicKeyTreeProofs()->at(integer(q,"oversizeProof"))->addProofChunk(false,MMRData(MiniData(std::vector<uint8_t>(65)),MiniNumber(1)));
}
static Object run(const std::shared_ptr<Transaction>& t,const MiniData& pub,const Object& q){
    if(q.containsKey("changeState"))t->addStateVariable(StateVariable(integer(q,"changeState"),"0x00"));
    if(flag(q,"changeInput"))t->getAllInputs()[0].resetCoinID(MiniData::fromHex("0xAB"));
    if(flag(q,"changeOutput"))t->getAllOutputs()[0].setStoreState(!t->getAllOutputs()[0].storeState());
    auto w=std::make_shared<Witness>();for(const auto& coin:t->getAllInputs())w->addCoinProof(CoinProof(coin,MMRProof()));
    kissvm::Contract c(SPHINCSUtils::getKISSVMScript(pub),std::vector<MiniData>{},w,t);c.setGlobalVariable("@TOTIN",std::make_shared<kissvm::NumberValue>(int64_t(t->getAllInputs().size())));c.setGlobalVariable("@TOTOUT",std::make_shared<kissvm::NumberValue>(int64_t(t->getAllOutputs().size())));c.run();
    Object r;r.put("valid",c.isSuccess());r.put("parse",c.isParseOK());r.put("exception",c.isException());r.put("error",c.getException());r.put("operations",c.getNumberOfInstructions());r.put("variables",c.getAllVariables());return r;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){
    auto q=std::get<std::shared_ptr<Object>>(json::JSONValue::parseWithException(line).value);Object r;std::shared_ptr<Transaction> t;int logs=0;
    MinimaLogger::Runtime runtime;runtime.println=[&](const auto& text){if(unicode::encodeUTF8(text).find("] : java.")!=std::string::npos)++logs;};MinimaLogger::setRuntime(std::move(runtime));
    try{
        auto action=q->get("action").asString();
        if(action=="script"){r.put("constant",SPHINCSUtils::KISSVM_SPHINCS_SCRIPT);r.put("script",SPHINCSUtils::getKISSVMScript(data(q->get("public")).get()));}
        else {t=transaction(*q);
            if(action=="message")r.put("message",SPHINCSUtils::calculateTransactionID(t.get()).to0xString());
            else {std::optional<SPHINCSSignature> s;auto pub=data(q->get("public"));
                if(action=="signSetup"){SPHINCS signer(data(q->get("seed")).get());auto message=SPHINCSUtils::calculateTransactionID(t.get());s=signer.signMessage(message);pub=signer.getPublicKey();r.put("message",message.to0xString());r.put("public",pub->to0xString());r.put("signature",MiniData(s->serialise()).to0xString());}
                else if(auto bytes=data(q->get("wire"))){size_t offset=0;s=SPHINCSSignature::deserialise(bytes->bytes(),offset);}
                if(s){if(q->containsKey("mutation"))mutate(*s->getFORSSignature(),*std::get<std::shared_ptr<Object>>(q->get("mutation").value));int mask=integer(*q,"nulls");s=SPHINCSSignature(mask&1?nullptr:s->getWOTSSignature(),mask&2?nullptr:s->getFORSRoot(),mask&4?nullptr:s->getFORSSignature());}
                SPHINCSUtils::setupTransaction(t.get(),s?&*s:nullptr);if(action=="run"||action=="signSetup"){if(!pub)throw JavaNullPointerException("");r.put("contract",run(t,*pub,*q));}
            }
        }
    }catch(const std::exception& e){r.put("error",error(e));}
    try{r.put("transaction",snapshot(t));}catch(const std::exception& e){r.put("snapshotError",error(e));}MinimaLogger::setRuntime({});r.put("logCount",logs);std::cout<<r.toJSONString()<<'\n';
}}
