#pragma once
#include "SignatureProof.hpp"
#include "support/JavaExceptions.hpp"
#include <optional>
namespace minima {
class Signature {
    std::vector<SignatureProof> mSignatures;
public:
    void addSignatureProof(SignatureProof signature) {mSignatures.push_back(std::move(signature));}
    const std::vector<SignatureProof>& getAllSignatureProofs() const {return mSignatures;}
    std::vector<SignatureProof>& getAllSignatureProofs() {return mSignatures;}
    MiniData getRootPublicKey() const {if(mSignatures.empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");return mSignatures[0].getRootPublicKey();}
    json::JSONObject toJSON() const;
    std::vector<uint8_t> serialise() const;
    static Signature deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<Signature> convertMiniDataVersion(const MiniData& data);
};
}
