#include "FORSSignature.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::sphincs::fors {
namespace {
template<class T> const T& at(const std::vector<std::shared_ptr<T>>& list,size_t index) {
    if(index>=list.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(index)+" out of bounds for length "+std::to_string(list.size()));
    if(!list[index])throw JavaNullPointerException("");return *list[index];
}
template<class T> std::optional<MiniData> miniDataVersion(const T& object) {
    try {return MiniData(object.serialise());}
    catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));return std::nullopt;}
}
}
json::JSONObject FORSSignature::toJSON() const {
    json::JSONObject result;result.put("horstsignature",mData->signature->toJSON());json::JSONArray roots;
    size_t count=mData->signature->getSignatureValues()->size();
    for(size_t i=0;i<count;++i) {
        json::JSONObject value;
        // This seemingly unused source call can log an IOException; a null
        // result is ignored, and root.toJSON still executes afterwards.
        (void)miniDataVersion(at(*mData->roots,i));
        value.put("horstroot",at(*mData->roots,i).toJSON());
        auto proof=miniDataVersion(at(*mData->proofs,i));
        if(!proof)throw JavaNullPointerException("");
        value.put("horstproof",proof->to0xString());roots.add(value);
    }
    result.put("horstroots",roots);return result;
}
void FORSSignature::writeDataStream(std::vector<uint8_t>& output) const {
    mData->signature->writeDataStream(output);size_t count=mData->signature->getSignatureValues()->size();
    wire::append(output,MiniNumber(int64_t(count)).serialise());
    for(size_t i=0;i<count;++i) {
        wire::append(output,at(*mData->roots,i).serialise());at(*mData->proofs,i).writeDataStream(output);
    }
}
std::vector<uint8_t> FORSSignature::serialise() const {std::vector<uint8_t> output;writeDataStream(output);return output;}
void FORSSignature::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->signature=std::make_shared<horst::HORSTSignature>();mData->signature->readDataStream(data,offset);
    // Root/proof lists must survive a failure in the embedded signature read.
    mData->roots=std::make_shared<Roots>();mData->proofs=std::make_shared<Proofs>();
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i) {
        mData->roots->push_back(std::make_shared<MMRData>(MMRData::deserialise(data,offset)));
        mData->proofs->push_back(std::make_shared<MMRProof>(MMRProof::deserialise(data,offset)));
    }
}
FORSSignature FORSSignature::deserialise(std::span<const uint8_t> data,size_t& offset) {FORSSignature signature;signature.readDataStream(data,offset);return signature;}
FORSSignature FORSSignature::convertMiniDataVersion(const MiniData* data) {
    if(!data)throw JavaNullPointerException("");FORSSignature signature;size_t offset=0;
    try {signature.readDataStream(data->bytes(),offset);}
    catch(const wire::IOException& error){MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    return signature;
}
}
