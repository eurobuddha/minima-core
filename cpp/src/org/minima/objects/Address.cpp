#include "Address.hpp"
#include "mmr/MMRData.hpp"
#include "org/minima/utils/BaseConverter.hpp"
#include "support/JavaExceptions.hpp"
namespace minima {
const Address Address::TRUE_ADDRESS("RETURN TRUE");
Address::Address(const std::string& script):mScript(script) {
    // A single-leaf MMR has an empty branch: calculating its proof returns the leaf.
    mAddressData=MMRData::CreateMMRDataLeafNode(mScript,MiniNumber(0)).getData();
    mMinimaAddress=makeMinimaAddress(mAddressData);
}
Address::Address(MiniData address):mAddressData(std::move(address)),mMinimaAddress(makeMinimaAddress(mAddressData)){}
json::JSONObject Address::toJSON() const {json::JSONObject obj;obj.put("script",mScript.toString());obj.put("hexaddress",mAddressData.to0xString());obj.put("miniaddress",mMinimaAddress);return obj;}
void Address::writeDataStream(std::vector<uint8_t>& out) const {mAddressData.writeHashToStream(out);mScript.writeDataStream(out);}
std::vector<uint8_t> Address::serialise() const {return Streamable::serialise();}
void Address::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mAddressData=MiniData::deserialiseHash(data,offset);mScript=MiniString::deserialise(data,offset);
    mMinimaAddress=makeMinimaAddress(mAddressData);
}
Address Address::deserialise(std::span<const uint8_t> data,size_t& offset){Address addr;addr.readDataStream(data,offset);return addr;}
std::string Address::makeMinimaAddress(const MiniData& address) {
    auto size=address.size();
    std::vector<uint8_t> envelope{1,uint8_t(size>>8),uint8_t(size)};
    wire::append(envelope,address.bytes());
    auto checksum=address.sha3();wire::append(envelope,std::span(checksum.bytes()).first(4));
    return BaseConverter::encode32(envelope);
}
MiniData Address::convertMinimaAddress(const std::string& address) {
    auto raw=BaseConverter::decode32(address);size_t offset=0;
    if(raw.empty()||wire::byte(raw,offset)!=1) throw std::invalid_argument("Invalid MxAddress - should start with 1 "+address);
    int size;
    try {size=int(wire::byte(raw,offset))<<8;size|=wire::byte(raw,offset);}
    catch(const wire::IOException&){throw std::invalid_argument("Invalid MxAddress : "+address+" java.io.EOFException");}
    if(size>=32768)throw JavaNegativeArraySizeException(std::to_string(size-65536));
    if(raw.size()-offset<size_t(size)+4)throw std::invalid_argument("Invalid MxAddress : "+address+" java.io.EOFException");
    auto data=MiniData::fromBytes(raw.data()+offset,size);offset+=size;
    auto hash=data.sha3();
    if(!std::equal(hash.bytes().begin(),hash.bytes().begin()+4,raw.begin()+offset)) throw std::invalid_argument("Invalid MxAddress - checksum wrong for "+address);
    return data;
}
}
