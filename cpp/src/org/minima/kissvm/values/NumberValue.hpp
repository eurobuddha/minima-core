#pragma once
#include "Value.hpp"
#include "org/minima/objects/base/MiniNumber.hpp"
namespace minima::kissvm {
class NumberValue : public Value {
protected:
    MiniNumber mNumber;
public:
    explicit NumberValue(int64_t value):mNumber(value){}
    explicit NumberValue(const std::string& value):mNumber(value){}
    explicit NumberValue(MiniNumber value):mNumber(std::move(value)){}
    const MiniNumber& getNumber() const {return mNumber;}
    int getValueType() const override {return VALUE_NUMBER;}
    bool isFalse() const {return mNumber.isEqual(MiniNumber(0));}
    std::string toString() const override;
    bool isEqual(const NumberValue& value) const {return mNumber.isEqual(value.mNumber);}
    bool isLess(const NumberValue& value) const {return mNumber.isLess(value.mNumber);}
    bool isLessEqual(const NumberValue& value) const {return mNumber.isLessEqual(value.mNumber);}
    bool isMore(const NumberValue& value) const {return mNumber.isMore(value.mNumber);}
    bool isMoreEqual(const NumberValue& value) const {return mNumber.isMoreEqual(value.mNumber);}
    NumberValue add(const NumberValue& value) const;
    NumberValue sub(const NumberValue& value) const;
    NumberValue mult(const NumberValue& value) const;
    NumberValue div(const NumberValue& value) const;
};
}
