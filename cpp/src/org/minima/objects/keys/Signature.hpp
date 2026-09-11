#pragma once
#include "org/minima/utils/Streamable.hpp"
#include "SignatureProof.hpp"
#include "support/JavaExceptions.hpp"
#include <optional>
namespace minima {
class Signature:public Streamable {
public:
    using Proofs=std::vector<SignatureProof>;
private:
    struct Data {std::shared_ptr<Proofs> signatures=std::make_shared<Proofs>();};
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    void addSignatureProof(SignatureProof signature) {mData->signatures->push_back(std::move(signature));}
    std::shared_ptr<Proofs> getAllSignatureProofs() const {return mData->signatures;}
    MiniData getRootPublicKey() const {if(mData->signatures->empty())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index 0 out of bounds for length 0");return (*mData->signatures)[0].getRootPublicKey();}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static Signature deserialise(std::span<const uint8_t> data,size_t& offset);
    static std::optional<Signature> convertMiniDataVersion(const MiniData& data);
};
}
