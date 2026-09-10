#pragma once
#include "MMRData.hpp"
#include <memory>
#include "org/minima/objects/base/MiniByte.hpp"
namespace minima {
class MMRProof {
public:
    struct MMRProofChunk {
        MiniByte mLeft;
        MMRData mMMRData;
        MMRProofChunk(bool left,MMRData data):mLeft(left),mMMRData(std::move(data)){}
        bool isLeft() const {return mLeft.isTrue();}
        const MMRData& getMMRData() const {return mMMRData;}
        json::JSONObject toJSON() const;
        std::vector<uint8_t> serialise() const;
        static MMRProofChunk deserialise(std::span<const uint8_t> data,size_t& offset);
    };
private:
    struct Data {MiniNumber mBlockTime;std::vector<MMRProofChunk> mProofChain;};
    std::shared_ptr<Data> mData=std::make_shared<Data>();
public:
    MMRProof()=default;
    explicit MMRProof(MiniNumber blockTime){mData->mBlockTime=std::move(blockTime);}
    const MiniNumber& getBlockTime() const {return mData->mBlockTime;}
    void addProofChunk(MMRProofChunk chunk) {mData->mProofChain.push_back(std::move(chunk));}
    void addProofChunk(bool left,MMRData data) {mData->mProofChain.emplace_back(left,std::move(data));}
    const MMRProofChunk& getProofChunk(size_t index) const {return mData->mProofChain.at(index);}
    int getProofLength() const {return int(mData->mProofChain.size());}
    MMRData calculateProof(MMRData data) const;
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset);
    static MMRProof deserialise(std::span<const uint8_t> data,size_t& offset);
    static MMRProof convertMiniDataVersion(const MiniData& data) {size_t offset=0;return deserialise(data.bytes(),offset);}
};
}
