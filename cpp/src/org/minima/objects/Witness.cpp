#include "Witness.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
void Witness::addSignature(Signature signature) {if(!isSignedBy(signature.getRootPublicKey().to0xString())) mData->signatures->push_back(std::move(signature));}
void Witness::addSignature(const std::shared_ptr<Signature>& signature) {if(!signature)throw JavaIllegalArgumentException("Cannot add a NULL Signature");addSignature(*signature);}
std::vector<MiniData> Witness::getAllSignatureKeys() const {std::vector<MiniData> keys;for(const auto& signature:*mData->signatures) keys.push_back(signature.getRootPublicKey());return keys;}
bool Witness::isSignedBy(const std::optional<std::string>& publicKey) const {for(const auto& key:getAllSignatureKeys()) if(publicKey&&key.to0xString()==*publicKey) return true;return false;}
void Witness::addScript(ScriptProof script) {if(!getScript(script.getAddressData())) mData->scripts->push_back(std::move(script));}
const ScriptProof* Witness::getScript(const MiniData& address) const {for(const auto& script:*mData->scripts) if(script.getAddressData().isEqual(address)) return &script;return nullptr;}
json::JSONObject Witness::toJSON() const {
    json::JSONObject obj;json::JSONArray sigs,coins,scripts;
    for(const auto& signature:*mData->signatures) sigs.add(signature.toJSON());
    for(const auto& coin:*mData->coins) coins.add(coin.toJSON());
    for(const auto& script:*mData->scripts) scripts.add(script.toJSON());
    obj.put("signatures",sigs);obj.put("mmrproofs",coins);obj.put("scripts",scripts);return obj;
}
void Witness::writeDataStream(std::vector<uint8_t>& out) const {
    MiniNumber(int64_t(mData->signatures->size())).writeDataStream(out);
    for(const auto& sig:*mData->signatures) sig.writeDataStream(out);
    wire::append(out,MiniNumber(int64_t(mData->coins->size())).serialise());
    for(const auto& coin:*mData->coins) coin.writeDataStream(out);
    wire::append(out,MiniNumber(int64_t(mData->scripts->size())).serialise());
    for(const auto& script:*mData->scripts) script.writeDataStream(out);
}
std::vector<uint8_t> Witness::serialise() const {return Streamable::serialise();}
void Witness::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->signatures=std::make_shared<Signatures>();int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mData->signatures->push_back(Signature::deserialise(data,offset));
    mData->coins=std::make_shared<CoinProofs>();count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mData->coins->push_back(CoinProof::deserialise(data,offset));
    mData->scripts=std::make_shared<Scripts>();count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mData->scripts->push_back(ScriptProof::deserialise(data,offset));
}
Witness Witness::deserialise(std::span<const uint8_t> data,size_t& offset) {Witness witness;witness.readDataStream(data,offset);return witness;}
}
