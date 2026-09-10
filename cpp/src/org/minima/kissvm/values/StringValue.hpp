#pragma once
#include "Value.hpp"
#include "org/minima/objects/base/MiniString.hpp"
namespace minima::kissvm {
class StringValue : public Value {
    MiniString mScript;
    std::u16string mUTF16;
public:
    explicit StringValue(const std::string& script);
    explicit StringValue(const std::u16string& script);
    const std::u16string& getUTF16() const {return mUTF16;}
    int getValueType() const override {return VALUE_SCRIPT;}
    std::string toString() const override;
    std::vector<uint8_t> getBytes() const {return mScript.getData();}
    const MiniString& getMiniString() const {return mScript;}
    bool isEqual(const StringValue& value) const {return mUTF16==value.mUTF16;}
    StringValue add(const StringValue& value) const;
};
}
