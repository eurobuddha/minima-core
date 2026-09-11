#include "MMRData.hpp"
namespace minima {
MMRData MMRData::CreateMMRDataParentNode(const MMRData& left,const MMRData& right) {
    auto sum=left.mValue.add(right.mValue);
    return MMRData(Crypto::getInstance().hashAllObjects(MiniNumber::ONE,left.mData,right.mData,sum),sum);
}
bool MMRData::isEqual(const MMRData& other) const {return mData.isEqual(other.mData)&&mValue.isEqual(other.mValue);}
json::JSONObject MMRData::toJSON() const {json::JSONObject obj;obj.put("data",mData.to0xString());obj.put("value",mValue.toString());return obj;}
void MMRData::writeDataStream(std::vector<uint8_t>& out) const {wire::append(out,mData.serialiseHash());wire::append(out,mValue.serialise());}
std::vector<uint8_t> MMRData::serialise() const {return Streamable::serialise();}
void MMRData::readDataStream(std::span<const uint8_t> data,size_t& offset){mData=MiniData::deserialiseHash(data,offset);mValue=MiniNumber::deserialise(data,offset);}
MMRData MMRData::deserialise(std::span<const uint8_t> data,size_t& offset) {
    auto hash=MiniData::deserialiseHash(data,offset);
    auto value=MiniNumber::deserialise(data,offset);
    return MMRData(std::move(hash),std::move(value));
}
}
