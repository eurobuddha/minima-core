#include "MMRProof.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
json::JSONObject MMRProof::MMRProofChunk::toJSON() const {json::JSONObject obj;obj.put("left",isLeft());obj.put("data",mMMRData.toJSON());return obj;}
void MMRProof::MMRProofChunk::writeDataStream(std::vector<uint8_t>& output) const {mLeft.writeDataStream(output);mMMRData.writeDataStream(output);}
void MMRProof::MMRProofChunk::readDataStream(std::span<const uint8_t> data,size_t& offset){mLeft=MiniByte::deserialise(data,offset);mMMRData=MMRData::deserialise(data,offset);}
std::vector<uint8_t> MMRProof::MMRProofChunk::serialise() const {return Streamable::serialise();}
MMRProof::MMRProofChunk MMRProof::MMRProofChunk::deserialise(std::span<const uint8_t> data,size_t& offset) {
    auto left=MiniByte::deserialise(data,offset);
    auto mmr=MMRData::deserialise(data,offset);
    MMRProofChunk chunk(left.isTrue(),std::move(mmr));chunk.mLeft=left;return chunk;
}
MMRData MMRProof::calculateProof(MMRData data) const {
    for(const auto& chunk:mData->mProofChain) data=chunk.isLeft()?MMRData::CreateMMRDataParentNode(chunk.mMMRData,data):MMRData::CreateMMRDataParentNode(data,chunk.mMMRData);
    return data;
}
json::JSONObject MMRProof::toJSON() const {
    json::JSONObject obj;obj.put("blocktime",mData->mBlockTime.toString());
    json::JSONArray arr;for(const auto& chunk:mData->mProofChain) arr.add(chunk.toJSON());
    obj.put("proof",arr);obj.put("prooflength",getProofLength());return obj;
}
void MMRProof::writeDataStream(std::vector<uint8_t>& output) const {
    wire::append(output,mData->mBlockTime.serialise());
    wire::append(output,MiniNumber(getProofLength()).serialise());
    for(const auto& chunk:mData->mProofChain)chunk.writeDataStream(output);
}
std::vector<uint8_t> MMRProof::serialise() const {std::vector<uint8_t> output;writeDataStream(output);return output;}
void MMRProof::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->mBlockTime=MiniNumber::deserialise(data,offset);
    mData->mProofChain.clear();
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i)addProofChunk(MMRProofChunk::deserialise(data,offset));
}
MMRProof MMRProof::deserialise(std::span<const uint8_t> data,size_t& offset) {
    MMRProof proof;proof.readDataStream(data,offset);return proof;
}
}
