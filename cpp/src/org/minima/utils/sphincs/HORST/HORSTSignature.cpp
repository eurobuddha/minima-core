#include "HORSTSignature.hpp"
#include "org/minima/utils/json/JSONArray.hpp"
#include "support/NativeThrowable.hpp"
namespace minima::sphincs::horst {
namespace {
template<class T> const T& required(const std::shared_ptr<T>& value) {
    if(!value)throw JavaNullPointerException("");return *value;
}
const std::shared_ptr<MMRProof>& proofAt(const HORSTSignature::Proofs& proofs,size_t i) {
    if(i>=proofs.size())throw JavaTypedException("java.lang.IndexOutOfBoundsException","Index "+std::to_string(i)+" out of bounds for length "+std::to_string(proofs.size()));
    return proofs[i];
}
}
json::JSONObject HORSTSignature::toJSON() const {
    json::JSONObject result;json::JSONArray values;
    for(size_t i=0;i<mData->values->size();++i) {
        json::JSONObject value;value.put("sigvalue",required((*mData->values)[i]).to0xString());
        const auto& proof=required(proofAt(*mData->proofs,i));
        std::optional<MiniData> encoded;
        try {encoded=MiniData(proof.serialise());}
        catch(const wire::IOException& error) {
            // MiniData.getMiniDataVersion logs IOException and returns null;
            // the following source to0xString call then throws NPE.
            MinimaLogger::log(std::make_shared<NativeThrowable>(error));
        }
        if(!encoded)throw JavaNullPointerException("");
        value.put("sigproof",encoded->to0xString());values.add(value);
    }
    result.put("values",values);return result;
}
void HORSTSignature::writeDataStream(std::vector<uint8_t>& output) const {
    wire::append(output,MiniNumber(int64_t(mData->values->size())).serialise());
    for(size_t i=0;i<mData->values->size();++i) {
        wire::append(output,required((*mData->values)[i]).serialise());
        const auto& proof=required(proofAt(*mData->proofs,i));
        wire::append(output,proof.getBlockTime().serialise());
        wire::append(output,MiniNumber(proof.getProofLength()).serialise());
        for(int c=0;c<proof.getProofLength();++c) {
            const auto& chunk=proof.getProofChunk(c);
            wire::append(output,chunk.mLeft.serialise());
            wire::append(output,chunk.getMMRData().serialise());
        }
    }
}
std::vector<uint8_t> HORSTSignature::serialise() const {std::vector<uint8_t> out;writeDataStream(out);return out;}
void HORSTSignature::readDataStream(std::span<const uint8_t> data,size_t& offset) {
    mData->values=std::make_shared<Values>();mData->proofs=std::make_shared<Proofs>();
    int count=MiniNumber::deserialise(data,offset).getAsInt();
    for(int i=0;i<count;++i) {
        mData->values->push_back(std::make_shared<MiniData>(MiniData::deserialise(data,offset)));
        mData->proofs->push_back(std::make_shared<MMRProof>(MMRProof::deserialise(data,offset)));
    }
}
HORSTSignature HORSTSignature::deserialise(std::span<const uint8_t> data,size_t& offset) {HORSTSignature signature;signature.readDataStream(data,offset);return signature;}
HORSTSignature HORSTSignature::convertMiniDataVersion(const MiniData* data) {
    if(!data)throw JavaNullPointerException("");
    HORSTSignature signature;size_t offset=0;
    try {signature.readDataStream(data->bytes(),offset);}
    catch(const wire::IOException& error) {MinimaLogger::log(std::make_shared<NativeThrowable>(error));}
    return signature;
}
}
