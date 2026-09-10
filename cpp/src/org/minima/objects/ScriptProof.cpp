#include "ScriptProof.hpp"
namespace minima {
ScriptProof::ScriptProof(const std::string& script):ScriptProof(script,MMRProof()){}
ScriptProof::ScriptProof(const std::string& script,MMRProof proof):mScript(script),mProof(std::move(proof)) {calculateAddress();}
void ScriptProof::calculateAddress() {auto leaf=MMRData::CreateMMRDataLeafNode(mScript,MiniNumber(0));mAddress=Address(mProof.calculateProof(leaf).getData());}
json::JSONObject ScriptProof::toJSON() const {json::JSONObject obj;obj.put("script",mScript.toString());obj.put("address",mAddress.getAddressData().to0xString());obj.put("proof",mProof.toJSON());return obj;}
std::vector<uint8_t> ScriptProof::serialise() const {return wire::join(mScript,mProof);}
ScriptProof ScriptProof::deserialise(std::span<const uint8_t> data,size_t& offset) {auto script=MiniString::deserialise(data,offset);auto proof=MMRProof::deserialise(data,offset);return ScriptProof(script.toString(),std::move(proof));}
}
