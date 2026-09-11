#include "Signature.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
json::JSONObject Signature::toJSON() const {json::JSONObject obj;json::JSONArray arr;for(const auto& sig:*mData->signatures) arr.add(sig.toJSON());obj.put("signatures",arr);return obj;}
void Signature::writeDataStream(std::vector<uint8_t>& output) const {
    wire::append(output,MiniNumber(int64_t(mData->signatures->size())).serialise());
    for(const auto& signature:*mData->signatures)signature.writeDataStream(output);
}
std::vector<uint8_t> Signature::serialise() const {std::vector<uint8_t> output;writeDataStream(output);return output;}
void Signature::readDataStream(std::span<const uint8_t> data,size_t& offset){mData->signatures=std::make_shared<Proofs>();int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)addSignatureProof(SignatureProof::deserialise(data,offset));}
Signature Signature::deserialise(std::span<const uint8_t> data,size_t& offset) {Signature sig;sig.readDataStream(data,offset);return sig;}
std::optional<Signature> Signature::convertMiniDataVersion(const MiniData& data) {try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}}
}
