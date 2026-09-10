#include "Witness.hpp"
#include "support/JavaExceptions.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
namespace minima {
void Witness::addSignature(Signature signature) {if(!isSignedBy(signature.getRootPublicKey().to0xString())) mSignatureProofs.push_back(std::move(signature));}
void Witness::addSignature(const std::shared_ptr<Signature>& signature) {if(!signature)throw JavaIllegalArgumentException("Cannot add a NULL Signature");addSignature(*signature);}
std::vector<MiniData> Witness::getAllSignatureKeys() const {std::vector<MiniData> keys;for(const auto& signature:mSignatureProofs) keys.push_back(signature.getRootPublicKey());return keys;}
bool Witness::isSignedBy(const std::optional<std::string>& publicKey) const {for(const auto& key:getAllSignatureKeys()) if(publicKey&&key.to0xString()==*publicKey) return true;return false;}
void Witness::addScript(ScriptProof script) {if(!getScript(script.getAddressData())) mScriptProofs.push_back(std::move(script));}
const ScriptProof* Witness::getScript(const MiniData& address) const {for(const auto& script:mScriptProofs) if(script.getAddressData().isEqual(address)) return &script;return nullptr;}
json::JSONObject Witness::toJSON() const {
    json::JSONObject obj;json::JSONArray sigs,coins,scripts;
    for(const auto& signature:mSignatureProofs) sigs.add(signature.toJSON());
    for(const auto& coin:mCoinProofs) coins.add(coin.toJSON());
    for(const auto& script:mScriptProofs) scripts.add(script.toJSON());
    obj.put("signatures",sigs);obj.put("mmrproofs",coins);obj.put("scripts",scripts);return obj;
}
std::vector<uint8_t> Witness::serialise() const {
    auto out=MiniNumber(int64_t(mSignatureProofs.size())).serialise();
    for(const auto& sig:mSignatureProofs) wire::append(out,sig.serialise());
    wire::append(out,MiniNumber(int64_t(mCoinProofs.size())).serialise());
    for(const auto& coin:mCoinProofs) wire::append(out,coin.serialise());
    wire::append(out,MiniNumber(int64_t(mScriptProofs.size())).serialise());
    for(const auto& script:mScriptProofs) wire::append(out,script.serialise());
    return out;
}
void Witness::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mSignatureProofs.clear();int count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mSignatureProofs.push_back(Signature::deserialise(data,offset));
    mCoinProofs.clear();count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mCoinProofs.push_back(CoinProof::deserialise(data,offset));
    mScriptProofs.clear();count=MiniNumber::deserialise(data,offset).getAsInt();for(int i=0;i<count;++i)mScriptProofs.push_back(ScriptProof::deserialise(data,offset));
}
Witness Witness::deserialise(std::span<const uint8_t> data,size_t& offset) {Witness witness;witness.readDataStream(data,offset);return witness;}
}
