#pragma once
#include "org/minima/objects/base/MiniNumber.hpp"
namespace minima {
class MMREntryNumber {
    BigInt mUnscaled;
    int32_t mScale=0;
    static BigInt scaleUp(const BigInt& value,int64_t places);
    static void align(const MMREntryNumber& a,const MMREntryNumber& b,BigInt& ua,BigInt& ub,int32_t& scale);
public:
    static const MMREntryNumber ZERO,TWO;
    MMREntryNumber()=default;
    explicit MMREntryNumber(int number):mUnscaled(number){}
    explicit MMREntryNumber(BigInt number):mUnscaled(std::move(number)){}
    MMREntryNumber(BigInt unscaled,int32_t scale):mUnscaled(std::move(unscaled)),mScale(scale){}
    const BigInt& getUnscaled() const {return mUnscaled;}
    int32_t getScale() const {return mScale;}
    MMREntryNumber modulo(const MMREntryNumber& other) const;
    MMREntryNumber floor() const;
    MMREntryNumber increment() const;
    MMREntryNumber decrement() const;
    MMREntryNumber div2() const;
    MMREntryNumber mult2() const;
    // Native BigDecimal adapters used for unbounded chain weights.
    MMREntryNumber add(const MMREntryNumber& other) const;
    MMREntryNumber multiplyDecimal32(const BigInt& factor) const;
    static MMREntryNumber dividePositive(const BigInt& numerator,const BigInt& denominator,int precision);
    BigInt toBigInteger() const;
    std::string toBigDecimalString() const;
    int compareTo(const MMREntryNumber& other) const;
    bool isEqual(const MMREntryNumber& other) const {return compareTo(other)==0;}
    bool isLess(const MMREntryNumber& other) const {return compareTo(other)<0;}
    bool isLessEqual(const MMREntryNumber& other) const {return compareTo(other)<=0;}
    bool isMore(const MMREntryNumber& other) const {return compareTo(other)>0;}
    bool isMoreEqual(const MMREntryNumber& other) const {return compareTo(other)>=0;}
    std::string toString() const;
    std::vector<uint8_t> serialise() const;
    static MMREntryNumber deserialise(std::span<const uint8_t> data,size_t& offset);
};
}
