#include "Signature.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
namespace minima {
json::JSONObject Signature::toJSON() const {json::JSONObject obj;json::JSONArray arr;for(const auto& sig:mSignatures) arr.add(sig.toJSON());obj.put("signatures",arr);return obj;}
std::vector<uint8_t> Signature::serialise() const {auto out=MiniNumber(int64_t(mSignatures.size())).serialise();for(const auto& sig:mSignatures) wire::append(out,sig.serialise());return out;}
Signature Signature::deserialise(std::span<const uint8_t> data,size_t& offset) {Signature sig;int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i) sig.addSignatureProof(SignatureProof::deserialise(data,offset));return sig;}
std::optional<Signature> Signature::convertMiniDataVersion(const MiniData& data) {try {size_t offset=0;return deserialise(data.bytes(),offset);}catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}}
}
