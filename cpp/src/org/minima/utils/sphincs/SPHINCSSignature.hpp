#pragma once
#include "org/minima/utils/Streamable.hpp"
#include "org/minima/objects/keys/Signature.hpp"
#include "FORS/FORSSignature.hpp"
namespace minima::sphincs {
class SPHINCSSignature:public Streamable {
    struct Data {
        std::shared_ptr<Signature> signature;
        std::shared_ptr<MMRData> root;
        std::shared_ptr<fors::FORSSignature> fors;
    };
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    SPHINCSSignature()=default;
    SPHINCSSignature(std::shared_ptr<Signature> signature,std::shared_ptr<MMRData> root,std::shared_ptr<fors::FORSSignature> fors);
    std::shared_ptr<Signature> getWOTSSignature() const {return mData->signature;}
    std::shared_ptr<MMRData> getFORSRoot() const {return mData->root;}
    std::shared_ptr<fors::FORSSignature> getFORSSignature() const {return mData->fors;}
    json::JSONObject toJSON() const;
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static SPHINCSSignature deserialise(std::span<const uint8_t> data,size_t& offset);
    static SPHINCSSignature convertMiniDataVersion(const MiniData* data);
    static SPHINCSSignature convertMiniDataVersion(const MiniData& data) {return convertMiniDataVersion(&data);}
};
}
