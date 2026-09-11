#include "org/minima/utils/Streamable.hpp"
#include "org/minima/utils/sphincs/SPHINCSSignature.hpp"
#include "org/minima/utils/MiniFile.hpp"
#include "org/minima/system/network/minima/NIOManager.hpp"
#include "support/MiniDataVersion.hpp"
#include <iostream>
#include <type_traits>
using namespace minima;using namespace minima::json;
static_assert(std::is_abstract_v<Streamable> && std::has_virtual_destructor_v<Streamable>);
static_assert(std::is_base_of_v<Streamable,MiniByte> && std::is_base_of_v<Streamable,MMRProof>);
static_assert(std::is_base_of_v<Streamable,sphincs::horst::HORSTSignature> && std::is_base_of_v<Streamable,sphincs::fors::FORSSignature> && std::is_base_of_v<Streamable,sphincs::SPHINCSSignature>);
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
    if(kind=="byte")return std::make_unique<MiniByte>();
    if(kind=="proof")return std::make_unique<MMRProof>();
    if(kind=="horst")return std::make_unique<sphincs::horst::HORSTSignature>();
    if(kind=="fors")return std::make_unique<sphincs::fors::FORSSignature>();
    if(kind=="sphincs")return std::make_unique<sphincs::SPHINCSSignature>();
    if(kind=="failing")return std::make_unique<Failing>(int(std::get<int64_t>(q.get("fail").value)));
    throw std::invalid_argument("Unknown fixture kind");
}
static wire::Bytes data(const JSONObject& q,const std::string& name){return MiniData::fromHex(q.getString(name)).bytes();}
static JSONObject state(const Streamable& value){JSONObject out;wire::Bytes bytes={0xaa,0x55};try{value.writeDataStream(bytes);}catch(const std::exception& e){out.put("writeError",category(e));}out.put("appended",MiniData(bytes).to0xString());return out;}
static JSONObject run(const JSONObject& q){
    JSONObject result;auto owner=create(q);Streamable& value=*owner;auto initial=data(q,"initial");size_t offset=0;value.readDataStream(initial,offset);auto op=q.getString("op");
    if(op=="read"){auto bytes=data(q,"wire");offset=size_t(std::get<int64_t>(q.get("offset").value));try{value.readDataStream(bytes,offset);}catch(const std::exception& e){result.put("readError",category(e));}result.put("offset",int64_t(offset));}
    else if(op=="callers"){
        result.put("data",miniDataVersion(value)->to0xString());result.put("hash",Crypto::getInstance().hashObject(value).to0xString());result.put("pairHash",Crypto::getInstance().hashObjects(value,value).to0xString());result.put("nio",NIOManager::createNIOMessage(MiniByte(13),value).to0xString());
        auto file=std::filesystem::path(q.getString("file"));try{MiniFile::writeObjectToFile(file,value);result.put("file",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeObjectToFile(file,value,true);result.put("appendedFile",MiniData(MiniFile::readCompleteFile(file)).to0xString());MiniFile::writeDataToFile(file,data(q,"wire"));MiniFile::loadObject(file,value);}catch(...){std::filesystem::remove(file);throw;}std::filesystem::remove(file);
    }
    result.put("state",state(value));return result;
}
int main(){std::cout<<std::unitbuf;std::string line;while(std::getline(std::cin,line)){JSONObject out;try{auto q=std::get<std::shared_ptr<JSONObject>>(JSONValue::parseWithException(line).value);out=run(*q);}catch(const std::exception& e){out.put("error",category(e));out.put("message",e.what());}std::cout<<out.toJSONString()<<'\n';}}
