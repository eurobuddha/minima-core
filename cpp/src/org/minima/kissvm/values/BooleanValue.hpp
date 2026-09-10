#pragma once
#include "Value.hpp"
namespace minima::kissvm {
class BooleanValue : public Value {
    bool mTrue;
public:
    static BooleanValue TRUE,FALSE;
    explicit BooleanValue(bool value):mTrue(value){}
    bool isTrue() const {return mTrue;}
    bool isFalse() const {return !mTrue;}
    std::string toString() const override;
    int getValueType() const override {return VALUE_BOOLEAN;}
    bool isEqual(const BooleanValue& value) const {return mTrue==value.mTrue;}
};
}
