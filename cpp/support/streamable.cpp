#include "org/minima/utils/Streamable.hpp"
#include "org/minima/utils/sphincs/SPHINCSSignature.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/objects/mmr/MMREntry.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "support/MiniDataVersion.hpp"
#include "org/minima/objects/base/MiniString.hpp"
#include <iostream>
#include <type_traits>
using namespace minima;using namespace minima::json;
static_assert(std::is_abstract_v<Streamable> && std::has_virtual_destructor_v<Streamable>);
static_assert(std::is_base_of_v<Streamable,MiniByte> && std::is_base_of_v<Streamable,MMRProof>);
static_assert(std::is_base_of_v<Streamable,sphincs::horst::HORSTSignature> && std::is_base_of_v<Streamable,sphincs::fors::FORSSignature> && std::is_base_of_v<Streamable,sphincs::SPHINCSSignature>);
static_assert(std::is_base_of_v<Streamable,MMRData> && std::is_base_of_v<Streamable,MMREntryNumber> && std::is_base_of_v<Streamable,MMREntry> && std::is_base_of_v<Streamable,MMRProof::MMRProofChunk> && std::is_base_of_v<Streamable,SignatureProof> && std::is_base_of_v<Streamable,Signature>);
static_assert(std::is_base_of_v<Streamable,MiniData> && std::is_base_of_v<Streamable,MiniNumber> && std::is_base_of_v<Streamable,MiniString>);
class Failing:public Streamable {
    uint8_t values[3]={1,2,3};int fail;
public:
    explicit Failing(int n):fail(n){}
    void writeDataStream(std::vector<uint8_t>& out) const override{for(int i=0;i<3;++i){if(i==fail)throw wire::IOException("fixture write");out.push_back(values[i]);}if(fail==3)throw wire::IOException("fixture write");}
    void readDataStream(std::span<const uint8_t> bytes,size_t& offset) override{for(auto& value:values)value=wire::byte(bytes,offset);}
};
static std::string category(const std::exception& e){auto text=nativeExceptionText(e);return text.substr(0,text.find(": "));}
static std::unique_ptr<Streamable> create(const JSONObject& q){
    auto kind=q.getString("kind");
    if(q.getString("op")=="badwrite"){
        MMRData d(MiniData(std::vector<uint8_t>(65)),MiniNumber(7));MMRProof p(MiniNumber(7));p.addProofChunk(true,d);SignatureProof s(MiniData::fromHex("0xA1"),MiniData::fromHex("0xB2"),p);
        if(kind=="mmrdata")return std::make_unique<MMRData>(d);
        if(kind=="entry")return std::make_unique<MMREntry>(7,MMREntryNumber(4),d);
        if(kind=="chunk")return std::make_unique<MMRProof::MMRProofChunk>(true,d);
        if(kind=="signatureproof")return std::make_unique<SignatureProof>(s);
        auto result=std::make_unique<Signature>();result->addSignatureProof(s);return result;
    }
    if(kind=="data")return std::make_unique<MiniData>();
    if(kind=="number")return std::make_unique<MiniNumber>();
    if(kind=="string")return std::make_unique<MiniString>();
    if(kind=="byte")return std::make_unique<MiniByte>();
    if(kind=="proof")return std::make_unique<MMRProof>();
    if(kind=="mmrdata")return std::make_unique<MMRData>(MiniData(),MiniNumber());
    if(kind=="entrynumber")return std::make_unique<MMREntryNumber>();
    if(kind=="entry"){if(q.containsKey("nullData")&&q.getBoolean("nullData"))return std::make_unique<MMREntry>(7,MMREntryNumber(4),std::shared_ptr<MMRData>());return std::make_unique<MMREntry>(7,MMREntryNumber(4));}
    if(kind=="chunk")return std::make_unique<MMRProof::MMRProofChunk>(false,MMRData(MiniData(),MiniNumber()));
    if(kind=="signatureproof")return std::make_unique<SignatureProof>(MiniData(),MiniData(),MMRProof());
    if(kind=="signature")return std::make_unique<Signature>();
    if(kind=="horst")return std::make_unique<sphincs::horst::HORSTSignature>();
    if(kind=="fors")return std::make_unique<sphincs::fors::FORSSignature>();
    if(kind=="sphincs")return std::make_unique<sphincs::SPHINCSSignature>();
    if(kind=="failing")return std::make_unique<Failing>(int(std::get<int64_t>(q.get("fail").value)));
    throw std::invalid_argument("Unknown fixture kind");
}
static wire::Bytes data(const JSONObject& q,const std::string& name){return MiniData::fromHex(q.getString(name)).bytes();}
static JSONObject state(const Streamable& value){JSONObject out;wire::Bytes bytes={0xaa,0x55};try{value.writeDataStream(bytes);}catch(const std::exception& e){out.put("writeError",category(e));}out.put("appended",MiniData(bytes).to0xString());return out;}
static JSONObject run(const JSONObject& q){
    JSONObject result;auto owner=create(q);Streamable& value=*owner;auto initial=data(q,"initial");size_t offset=0;if(q.getString("op")!="badwrite"&&(!q.containsKey("empty")||!q.getBoolean("empty")))value.readDataStream(initial,offset);if(auto d=dynamic_cast<MMRData*>(&value))d->setUnspendable(true);std::optional<Signature> alias;if(auto sig=dynamic_cast<Signature*>(&value))alias=*sig;auto oldList=alias?alias->getAllSignatureProofs():nullptr;auto op=q.getString("op");if(q.containsKey("cache")&&q.getBoolean("cache"))dynamic_cast<MiniData&>(value).getDataValue();
    if(op=="read"){auto storage=data(q,"wire");std::span<const uint8_t> bytes=q.containsKey("selfInput")?std::span<const uint8_t>(dynamic_cast<MiniData&>(value).bytes()):std::span<const uint8_t>(storage);offset=size_t(std::get<int64_t>(q.get("offset").value));try{if(q.containsKey("hashRead"))dynamic_cast<MiniData&>(value).readHashFromStream(bytes,offset);else if(q.containsKey("expectedSize"))dynamic_cast<MiniData&>(value).readDataStream(bytes,offset,int(std::get<int64_t>(q.get("expectedSize").value)));else value.readDataStream(bytes,offset);}catch(const std::exception& e){result.put("readError",category(e));}result.put("offset",int64_t(offset));}
    else if(op=="callers"){
        result.put("data",miniDataVersion(value)->to0xString());result.put("hash",Crypto::getInstance().hashObject(value).to0xString());result.put("pairHash",Crypto::getInstance().hashObjects(value,value).to0xString());result.put("nio",NIOManager::createNIOMessage(MiniByte(13),value).to0xString());
        auto file=std::filesystem::path(q.getString("file"));try{MiniFile::writeObjectToFile(file,value);result.put("file",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeObjectToFile(file,value,true);result.put("appendedFile",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeDataToFile(file,data(q,"wire"));MiniFile::loadObject(file,value);}catch(...){std::filesystem::remove(file);throw;}std::filesystem::remove(file);
    }
    if(auto e=dynamic_cast<MMREntry*>(&value))result.put("empty",e->isEmpty());if(auto d=dynamic_cast<MMRData*>(&value))result.put("unspendable",d->isUnspendable());if(alias){result.put("oldListSize",int64_t(oldList->size()));result.put("sameList",oldList==alias->getAllSignatureProofs());result.put("aliasCount",int64_t(alias->getAllSignatureProofs()->size()));}
    if(auto d=dynamic_cast<MiniData*>(&value)){result.put("numeric",d->getDataValue().toDecimalString());result.put("less128",d->isLess(MiniData::fromHex("0x80")));result.put("shiftLeft",d->shiftl(1).to0xString());result.put("shiftRight",d->shiftr(1).to0xString());}
    result.put("state",state(value));return result;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){JSONObject out;try{auto q=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);out=run(*q);}catch(const std::exception& e){out.put("error",category(e));out.put("message",e.what());}std::cout<<out.toJSONString()<<'\n';}}
