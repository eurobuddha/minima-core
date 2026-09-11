#pragma once
#include "org/minima/utils/Streamable.hpp"
#include "org/minima/objects/mmr/MMRProof.hpp"
namespace minima {
class SignatureProof:public Streamable {
    MiniData mPublicKey,mSignature;
    MMRProof mProof;
public:
    SignatureProof(MiniData publicKey,MiniData signature,MMRProof proof):mPublicKey(std::move(publicKey)),mSignature(std::move(signature)),mProof(std::move(proof)){}
    const MiniData& getPublicKey() const {return mPublicKey;}
    const MiniData& getSignature() const {return mSignature;}
    const MMRProof& getProof() const {return mProof;}
    MiniData getRootPublicKey() const;
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static SignatureProof deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
