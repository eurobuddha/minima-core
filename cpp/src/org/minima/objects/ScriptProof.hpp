#pragma once
#include "Address.hpp"
#include "mmr/MMRProof.hpp"
namespace minima {
class ScriptProof:public Streamable {
    MiniString mScript;
    MMRProof mProof;
    Address mAddress;
    void calculateAddress();
    ScriptProof()=default;
public:
    explicit ScriptProof(const std::string& script);
    ScriptProof(const std::string& script,MMRProof proof);
    const MiniString& getScript() const {return mScript;}
    const MMRProof& getProof() const {return mProof;}
    const Address& getAddress() const {return mAddress;}
    const MiniData& getAddressData() const {return mAddress.getAddressData();}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static ScriptProof deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
