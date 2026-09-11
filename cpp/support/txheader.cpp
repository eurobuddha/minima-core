#include "org/minima/objects/TxPoW.hpp"
#include "org/minima/system/params/GlobalParams.hpp"
#include "support/NativeThrowable.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include <iostream>
using namespace minima;using namespace minima::json;
static int integer(const JSONObject& q,const std::string& k,int fallback){return q.containsKey(k)?int(std::get<int64_t>(q.get(k).value)):fallback;}
static std::string category(const std::exception& e){auto s=nativeExceptionText(e);return s.substr(0,s.find(':'));}
static const std::vector<std::string> names={"nonce","chain","time","block","difficulty","root","total","magic","custom","body"};
static std::shared_ptr<Streamable> field(const TxHeader& h,int i){switch(i){case 0:return h.mNonce();case 1:return h.mChainID();case 2:return h.mTimeMilli();case 3:return h.mBlockNumber();case 4:return h.mBlockDifficulty();case 5:return h.mMMRRoot();case 6:return h.mMMRTotal();case 7:return h.mMagic();case 8:return h.mCustomHash();default:return h.mTxBodyHash();}}
static void set(TxHeader& h,int i,std::shared_ptr<Streamable> value){switch(i){case 0:h.mNonce()=std::dynamic_pointer_cast<MiniNumber>(value);break;case 1:h.mChainID()=std::dynamic_pointer_cast<MiniData>(value);break;case 2:h.mTimeMilli()=std::dynamic_pointer_cast<MiniNumber>(value);break;case 3:h.mBlockNumber()=std::dynamic_pointer_cast<MiniNumber>(value);break;case 4:h.mBlockDifficulty()=std::dynamic_pointer_cast<MiniData>(value);break;case 5:h.mMMRRoot()=std::dynamic_pointer_cast<MiniData>(value);break;case 6:h.mMMRTotal()=std::dynamic_pointer_cast<MiniNumber>(value);break;case 7:h.mMagic()=std::dynamic_pointer_cast<Magic>(value);break;case 8:h.mCustomHash()=std::dynamic_pointer_cast<MiniData>(value);break;default:h.mTxBodyHash()=std::dynamic_pointer_cast<MiniData>(value);}}
static JSONValue fieldWire(const std::shared_ptr<Streamable>& p){if(!p)return nullptr;try{return MiniData(p->serialise()).to0xString();}catch(const std::exception& e){return category(e);}}
static JSONArray parents(const std::shared_ptr<TxHeader::Parents>& p){JSONArray a;if(p)for(const auto& value:*p)a.add(value?JSONValue(value->to0xString()):JSONValue(nullptr));return a;}
static JSONObject snapshot(const TxHeader& h){JSONObject r;JSONArray f,aliases;for(int i=0;i<10;++i)f.add(fieldWire(field(h,i)));r.put("fields",f);r.put("parents",h.mSuperParents()?JSONValue(parents(h.mSuperParents())):JSONValue(nullptr));if(h.mSuperParents())for(const auto& a:*h.mSuperParents())for(const auto& b:*h.mSuperParents())aliases.add(a==b);r.put("parentAliases",aliases);return r;}
struct Restore {int levels=GlobalParams::MINIMA_CASCADE_LEVELS;std::shared_ptr<MiniData> main=TxHeader::MAIN_NET,test=TxHeader::TEST_NET;MiniNumber zero=MiniNumber::ZERO;MiniData mainBytes=*main,empty=MiniData::ZERO_TXPOWID,max=Crypto::MAX_HASH;~Restore(){GlobalParams::MINIMA_CASCADE_LEVELS=levels;TxHeader::MAIN_NET=main;TxHeader::TEST_NET=test;*main=mainBytes;*MiniNumber::sharedZero()=zero;*MiniData::sharedZeroTxPoWID()=empty;*Crypto::sharedMaxHash()=max;}};
template<class T>class Fault:public T {public:std::function<void()> action;void writeDataStream(std::vector<uint8_t>& out)const override{out.push_back(0xAB);if(action)action();throw wire::IOException("fixture");}};
class HashFault:public MiniData {public:std::function<void()> action;void writeHashToStream(std::vector<uint8_t>& out)const override{out.push_back(0xBC);if(action)action();throw wire::IOException("fixture");}};
static JSONObject run(const JSONObject& q){
 Restore restore;GlobalParams::MINIMA_CASCADE_LEVELS=integer(q,"levels",4);JSONObject r;auto op=q.getString("op");
 if(op=="convert"){
  JSONArray logs;MinimaLogger::Runtime runtime;runtime.println=[&](const auto& text){auto line=unicode::encodeUTF8(text);auto pos=line.find("] : java.");if(pos!=line.npos){auto type=line.substr(pos+4);logs.add(type.substr(0,type.find(": ")));}};MinimaLogger::setRuntime(std::move(runtime));
  auto input=MiniData::fromHex(q.getString("wire"));try{auto converted=TxHeader::convertMiniDataVersion(q.getBoolean("nullInput")?nullptr:&input);r.put("converted",bool(converted));if(converted)r.put("wire",MiniData(converted->serialise()).to0xString());}catch(const std::exception& e){r.put("error",category(e));}MinimaLogger::setRuntime({});r.put("logs",logs);return r;
 }
 if(op=="construct"){try{TxHeader h;r.put("length",int64_t(h.mSuperParents()->size()));}catch(const std::exception& e){r.put("error",category(e));}return r;}
 if(op=="defaults"){
  TxHeader a,b;auto chain=a.mChainID();TxHeader::MAIN_NET=std::make_shared<MiniData>(MiniData::fromHex("0x77"));TxHeader c;JSONArray checks;checks.add(a.mChainID()==b.mChainID());checks.add(a.mChainID()!=c.mChainID());checks.add(c.mChainID()==TxHeader::MAIN_NET);checks.add(a.mBlockDifficulty()==Crypto::sharedMaxHash());checks.add(a.mMMRTotal()==MiniNumber::sharedZero());checks.add(a.mCustomHash()==MiniData::sharedZeroTxPoWID());checks.add(a.mNonce()!=b.mNonce());checks.add(a.mNonce()!=a.mBlockNumber());checks.add(a.mMMRRoot()!=a.mTxBodyHash());checks.add(a.mMagic()!=b.mMagic());checks.add(a.mSuperParents()!=b.mSuperParents());checks.add((*a.mSuperParents())[0]!=(*a.mSuperParents())[1]);r.put("checks",checks);
  *chain=MiniData::fromHex("0x66");*a.mBlockDifficulty()=MiniData::fromHex("0x55");*a.mMMRTotal()=MiniNumber(44);*a.mCustomHash()=MiniData::fromHex("0x33");JSONArray values;values.add(b.mChainID()->to0xString());values.add(c.mChainID()->to0xString());values.add(Crypto::MAX_HASH.to0xString());values.add(MiniNumber::ZERO.toString());values.add(MiniData::ZERO_TXPOWID.to0xString());r.put("values",values);return r;
 }
 auto h=std::make_shared<TxHeader>();auto initial=MiniData::fromHex(q.getString("initial"));size_t start=0;h->readDataStream(initial.bytes(),start);
 if(q.containsKey("arrayLength")){int n=integer(q,"arrayLength",4);if(n<0)h->mSuperParents().reset();else{auto p=std::make_shared<TxHeader::Parents>();for(int i=0;i<n;++i)p->push_back(std::make_shared<MiniData>(MiniData::fromHex("0x33")));h->mSuperParents()=p;}}
 if(q.containsKey("nullParent"))TxHeader::parentAt(h->mSuperParents(),integer(q,"nullParent",0)).reset();
 if(q.containsKey("nullField"))set(*h,integer(q,"nullField",0),nullptr);
 GlobalParams::MINIMA_CASCADE_LEVELS=integer(q,"runLevels",4);
 if(op=="read"){
  std::vector<std::shared_ptr<Streamable>> old;for(int i=0;i<10;++i)old.push_back(field(*h,i));auto oldParents=h->mSuperParents();TxHeader alias=*h;auto input=MiniData::fromHex(q.getString("wire"));size_t offset=size_t(integer(q,"offset",0));
  try{h->readDataStream(input.bytes(),offset);}catch(const std::exception& e){r.put("error",category(e));}
  r.put("offset",int64_t(offset));r.put("state",snapshot(*h));r.put("alias",snapshot(alias));JSONArray same;for(int i=0;i<10;++i)same.add(old[i]==field(*h,i));r.put("sameFields",same);r.put("sameArray",oldParents==h->mSuperParents());r.put("oldParents",oldParents?JSONValue(parents(oldParents)):JSONValue(nullptr));return r;
 }
 if(op=="alias"){
  TxHeader alias=*h;auto old=h->getBodyHash();auto oldArray=h->mSuperParents();h->mTxBodyHash()=std::make_shared<MiniData>(MiniData::fromHex("0x99"));(*oldArray)[0]=(*oldArray)[1];h->mSuperParents()=std::make_shared<TxHeader::Parents>();JSONArray checks;checks.add(alias.getBodyHash()==h->getBodyHash());checks.add(old!=h->getBodyHash());checks.add(oldArray!=alias.mSuperParents());checks.add((*oldArray)[0]==(*oldArray)[1]);r.put("checks",checks);r.put("oldBody",old->to0xString());return r;
 }
 if(op=="write"){
  std::shared_ptr<Streamable> fault;int index=integer(q,"fault",-1);if(index>=0){std::weak_ptr<TxHeader> weak=h;auto action=[weak,index,&q]{if(auto h=weak.lock();h&&q.getBoolean("replace")){if(index>=10)TxHeader::parentAt(h->mSuperParents(),index-10)=std::make_shared<MiniData>(MiniData::fromHex("0x99"));else if(index==7)set(*h,index,std::make_shared<Magic>());else if(index==0||index==2||index==3||index==6)set(*h,index,std::make_shared<MiniNumber>(99));else set(*h,index,std::make_shared<MiniData>(MiniData::fromHex("0x99")));}};
   if(index==0||index==2||index==3||index==6){auto f=std::make_shared<Fault<MiniNumber>>();f->action=action;fault=f;}else if(index==7){auto f=std::make_shared<Fault<Magic>>();f->action=action;fault=f;}else if(index==1||index==4){auto f=std::make_shared<Fault<MiniData>>();f->action=action;fault=f;}else{auto f=std::make_shared<HashFault>();f->action=action;fault=f;}
   if(index>=10)TxHeader::parentAt(h->mSuperParents(),index-10)=std::dynamic_pointer_cast<MiniData>(fault);else set(*h,index,fault);fault.reset();
  }
  std::vector<uint8_t> output{0xAA,0x55};try{h->writeDataStream(output);}catch(const std::exception& e){r.put("error",category(e));}r.put("wire",MiniData(output).to0xString());if(index>=0&&q.getBoolean("replace"))r.put("replaced",index>=10?TxHeader::parentAt(h->mSuperParents(),index-10)->to0xString():fieldWire(field(*h,index)));return r;
 }
 if(op=="json"){try{r.put("json",h->toJSON());}catch(const std::exception& e){r.put("error",category(e));}return r;}
 throw std::invalid_argument("fixture operation");
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line))try{auto q=JSONValue::parseWithException(line);std::cout<<run(*std::get<std::shared_ptr<JSONObject>>(q.value)).toJSONString()<<'\n';}catch(const std::exception& e){std::cerr<<nativeExceptionText(e)<<'\n';return 1;}}
