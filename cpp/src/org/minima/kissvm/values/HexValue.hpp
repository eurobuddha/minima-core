#pragma once
#include "Value.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
namespace minima::kissvm {
class HexValue : public Value {
protected:
    MiniData mData;
public:
    explicit HexValue(const std::string& value):HexValue(MiniData::fromHex(value)){}
    explicit HexValue(MiniData value);
    explicit HexValue(std::vector<uint8_t> value):HexValue(MiniData(std::move(value))){}
    explicit HexValue(const MiniNumber& value);
    const MiniData& getMiniData() const {return mData;}
    const std::vector<uint8_t>& getRawData() const {return mData.bytes();}
    int getValueType() const override {return VALUE_HEX;}
    bool isEqual(const HexValue& value) const {return mData.isEqual(value.mData);}
    std::string toString() const override;
};
}
