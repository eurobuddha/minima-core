#include "SPHINCSSignature.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::sphincs {
namespace {
template<class T> const T& required(const std::shared_ptr<T>& value) {if(!value)throw JavaNullPointerException("");return *value;}
}
SPHINCSSignature::SPHINCSSignature(std::shared_ptr<Signature> signature,std::shared_ptr<MMRData> root,std::shared_ptr<fors::FORSSignature> fors) {
    mData->signature=std::move(signature);mData->root=std::move(root);mData->fors=std::move(fors);
}
json::JSONObject SPHINCSSignature::toJSON() const {
    json::JSONObject result;
    result.put("WOTSsignature",required(mData->signature).toJSON());
    result.put("FORSroot",required(mData->root).toJSON());
    result.put("FORSsignature",required(mData->fors).toJSON());return result;
}
void SPHINCSSignature::writeDataStream(std::vector<uint8_t>& output) const {
    required(mData->signature).writeDataStream(output);
    wire::append(output,required(mData->root).serialise());
    required(mData->fors).writeDataStream(output);
}
std::vector<uint8_t> SPHINCSSignature::serialise() const {std::vector<uint8_t> output;writeDataStream(output);return output;}
void SPHINCSSignature::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    // The first two source assignments happen only after a complete read. The
    // third happens before its nested read and therefore exposes partial state.
    mData->signature=std::make_shared<Signature>(Signature::deserialise(data,offset));
    mData->root=std::make_shared<MMRData>(MMRData::deserialise(data,offset));
    mData->fors=std::make_shared<fors::FORSSignature>();mData->fors->readDataStream(data,offset);
}
SPHINCSSignature SPHINCSSignature::deserialise(std::span<const uint8_t> data,size_t& offset) {SPHINCSSignature value;value.readDataStream(data,offset);return value;}
SPHINCSSignature SPHINCSSignature::convertMiniDataVersion(const MiniData* data) {
    if(!data)throw JavaNullPointerException("");SPHINCSSignature value;size_t offset=0;
    try {value.readDataStream(data->bytes(),offset);}
    catch(const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    return value;
}
}
