#pragma once
#include "org/minima/utils/sphincs/HORST/HORSTSignature.hpp"
namespace minima::sphincs::fors {
class FORSSignature {
public:
    using Roots=std::vector<std::shared_ptr<MMRData>>;
    using Proofs=std::vector<std::shared_ptr<MMRProof>>;
private:
    struct Data {
        std::shared_ptr<horst::HORSTSignature> signature=std::make_shared<horst::HORSTSignature>();
        std::shared_ptr<Roots> roots=std::make_shared<Roots>();
        std::shared_ptr<Proofs> proofs=std::make_shared<Proofs>();
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    std::shared_ptr<horst::HORSTSignature> getHORSTSignature() const {return mData->signature;}
    std::shared_ptr<Roots> getHORSTRoots() const {return mData->roots;}
    std::shared_ptr<Proofs> getHORSTTreeProofs() const {return mData->proofs;}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static FORSSignature deserialise(std::span<const uint8_t> data,size_t& offset);
    static FORSSignature convertMiniDataVersion(const MiniData* data);
    static FORSSignature convertMiniDataVersion(const MiniData& data) {return convertMiniDataVersion(&data);}
};
}
