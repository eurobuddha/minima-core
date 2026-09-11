#pragma once
#include "org/minima/utils/Streamable.hpp"
#include "org/minima/utils/Crypto.hpp"
#include "org/minima/utils/json/JSONObject.hpp"
namespace minima {
class MMRData:public Streamable {
    MiniData mData;
    MiniNumber mValue;
public:
    bool mUnspendable=false;
    MMRData(MiniData hash,MiniNumber value):mData(std::move(hash)),mValue(std::move(value)){}
    template<class T> static MMRData CreateMMRDataLeafNode(const T& data,const MiniNumber& sum) {
        return MMRData(Crypto::getInstance().hashAllObjects(MiniNumber(0),data,sum),sum);
    }
    static MMRData CreateMMRDataParentNode(const MMRData& left,const MMRData& right);
    const MiniData& getData() const {return mData;}
    const MiniNumber& getValue() const {return mValue;}
    void setUnspendable(bool value) {mUnspendable=value;}
    bool isUnspendable() const {return mUnspendable;}
    bool isEqual(const MMRData& other) const;
    json::JSONObject toJSON() const;
    std::string toString() const {return toJSON().toJSONString();}
    void writeDataStream(std::vector<uint8_t>& output) const override;
    std::vector<uint8_t> serialise() const;
    void readDataStream(std::span<const uint8_t> data,size_t& offset) override;
    static MMRData deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
