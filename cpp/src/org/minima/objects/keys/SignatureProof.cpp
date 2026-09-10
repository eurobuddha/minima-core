#include "SignatureProof.hpp"
namespace minima {
MiniData SignatureProof::getRootPublicKey() const {return mProof.calculateProof(MMRData::CreateMMRDataLeafNode(mPublicKey,MiniNumber(0))).getData();}
json::JSONObject SignatureProof::toJSON() const {json::JSONObject obj;obj.put("publickey",mPublicKey.to0xString());obj.put("rootkey",getRootPublicKey().to0xString());obj.put("proof",mProof.toJSON());obj.put("signature",mSignature.to0xString());return obj;}
std::vector<uint8_t> SignatureProof::serialise() const {return wire::join(mPublicKey,mSignature,mProof);}
SignatureProof SignatureProof::deserialise(std::span<const uint8_t> data,size_t& offset) {auto key=MiniData::deserialise(data,offset);auto signature=MiniData::deserialise(data,offset);auto proof=MMRProof::deserialise(data,offset);return SignatureProof(std::move(key),std::move(signature),std::move(proof));}
}
