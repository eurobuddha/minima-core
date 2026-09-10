#pragma once
#include "Address.hpp"
#include "mmr/MMRProof.hpp"
namespace minima {
class ScriptProof {
    MiniString mScript;
    MMRProof mProof;
    Address mAddress;
    void calculateAddress();
public:
    explicit ScriptProof(const std::string& script);
    ScriptProof(const std::string& script,MMRProof proof);
    const MiniString& getScript() const {return mScript;}
    const MMRProof& getProof() const {return mProof;}
    const Address& getAddress() const {return mAddress;}
    const MiniData& getAddressData() const {return mAddress.getAddressData();}
    json::JSONObject toJSON() const;
    std::vector<uint8_t> serialise() const;
    static ScriptProof deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
