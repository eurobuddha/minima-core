#pragma once
#include "org/minima/objects/mmr/MMRProof.hpp"
namespace minima::sphincs::horst {
class HORSTSignature {
public:
    using Values=std::vector<std::shared_ptr<MiniData>>;
    using Proofs=std::vector<std::shared_ptr<MMRProof>>;
private:
    struct Data {
        std::shared_ptr<Values> values=std::make_shared<Values>();
        std::shared_ptr<Proofs> proofs=std::make_shared<Proofs>();
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    std::shared_ptr<Values> getSignatureValues() const {return mData->values;}
    std::shared_ptr<Proofs> getPublicKeyTreeProofs() const {return mData->proofs;}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static HORSTSignature deserialise(std::span<const uint8_t> data,size_t& offset);
    static HORSTSignature convertMiniDataVersion(const MiniData* data);
    static HORSTSignature convertMiniDataVersion(const MiniData& data) {return convertMiniDataVersion(&data);}
};
}
