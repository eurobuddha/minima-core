#include "SignatureProof.hpp"
namespace minima {
MiniData SignatureProof::getRootPublicKey() const {return mProof.calculateProof(MMRData::CreateMMRDataLeafNode(mPublicKey,MiniNumber(0))).getData();}
json::JSONObject SignatureProof::toJSON() const {json::JSONObject obj;obj.put("publickey",mPublicKey.to0xString());obj.put("rootkey",getRootPublicKey().to0xString());obj.put("proof",mProof.toJSON());obj.put("signature",mSignature.to0xString());return obj;}
void SignatureProof::writeDataStream(std::vector<uint8_t>& output) const {
    wire::append(output,mPublicKey.serialise());wire::append(output,mSignature.serialise());mProof.writeDataStream(output);
}
std::vector<uint8_t> SignatureProof::serialise() const {std::vector<uint8_t> output;writeDataStream(output);return output;}
void SignatureProof::readDataStream(std::span<const uint8_t> data,size_t& offset){mPublicKey=MiniData::deserialise(data,offset);mSignature=MiniData::deserialise(data,offset);mProof=MMRProof::deserialise(data,offset);}
SignatureProof SignatureProof::deserialise(std::span<const uint8_t> data,size_t& offset) {auto key=MiniData::deserialise(data,offset);auto signature=MiniData::deserialise(data,offset);auto proof=MMRProof::deserialise(data,offset);return SignatureProof(std::move(key),std::move(signature),std::move(proof));}
}
