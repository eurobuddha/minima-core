#include "MMREntryNumber.hpp"
#include "org/minima/objects/base/MiniData.hpp"
#include "support/ByteIO.hpp"
#include "support/JavaExceptions.hpp"
#include "support/JavaBigDecimalText.hpp"
#include <limits>
namespace minima {
const MMREntryNumber MMREntryNumber::ZERO(0),MMREntryNumber::TWO(2);
BigInt MMREntryNumber::scaleUp(const BigInt& value,int64_t places) {
    if(places<0) throw std::invalid_argument("Negative decimal expansion");
    if(value.isZero()) return value;
    BigInt multiplier(1),base(10);
    while(places) {if(places&1) multiplier=multiplier*base;places>>=1;if(places) base=base*base;}
    return value*multiplier;
}
void MMREntryNumber::align(const MMREntryNumber& a,const MMREntryNumber& b,BigInt& ua,BigInt& ub,int32_t& scale) {
    scale=std::max(a.mScale,b.mScale);ua=scaleUp(a.mUnscaled,int64_t(scale)-a.mScale);ub=scaleUp(b.mUnscaled,int64_t(scale)-b.mScale);
}
MMREntryNumber MMREntryNumber::modulo(const MMREntryNumber& other) const {
    BigInt a,b;int32_t scale;align(*this,other,a,b,scale);
    auto quotient=a/b;
    const int64_t preferred=int64_t(mScale)-other.mScale;
    int64_t quotientScale=0;
    if(quotient.isZero()) quotientScale=preferred;
    else if(preferred>0) {quotient=scaleUp(quotient,preferred);quotientScale=preferred;}
    else while(quotientScale>preferred && (quotient%BigInt(10)).isZero()) {quotient=quotient/BigInt(10);--quotientScale;}
    const auto productScale=quotientScale+other.mScale;
    if(productScale<std::numeric_limits<int32_t>::min() || productScale>std::numeric_limits<int32_t>::max()) throw std::overflow_error("Decimal scale overflow");
    MMREntryNumber product(quotient*other.mUnscaled,int32_t(productScale));
    align(*this,product,a,b,scale);
    return MMREntryNumber(a-b,scale);
}
MMREntryNumber MMREntryNumber::floor() const {
    if(mScale<=0) return MMREntryNumber(scaleUp(mUnscaled,-int64_t(mScale)));
    auto divisor=scaleUp(BigInt(1),mScale);auto integer=mUnscaled/divisor;
    if(mUnscaled.isNegative() && !(mUnscaled%divisor).isZero()) integer=integer-BigInt(1);
    return MMREntryNumber(integer);
}
MMREntryNumber MMREntryNumber::increment() const {BigInt a,b;int32_t scale;align(*this,MMREntryNumber(1),a,b,scale);return MMREntryNumber(a+b,scale);}
MMREntryNumber MMREntryNumber::decrement() const {BigInt a,b;int32_t scale;align(*this,MMREntryNumber(1),a,b,scale);return MMREntryNumber(a-b,scale);}
MMREntryNumber MMREntryNumber::div2() const {
    if((mUnscaled%BigInt(2)).isZero()) return MMREntryNumber(mUnscaled/BigInt(2),mScale);
    if(mScale==std::numeric_limits<int32_t>::max()) throw std::overflow_error("Decimal scale overflow");
    return MMREntryNumber(mUnscaled*BigInt(5),mScale+1);
}
MMREntryNumber MMREntryNumber::mult2() const {return MMREntryNumber(mUnscaled*BigInt(2),mScale);}
BigInt MMREntryNumber::toBigInteger() const {
    return mScale<=0?scaleUp(mUnscaled,-int64_t(mScale)):mUnscaled/scaleUp(BigInt(1),mScale);
}
MMREntryNumber MMREntryNumber::dividePositive(const BigInt& numerator,const BigInt& denominator,int precision) {
    // Shared BigDecimal HALF_EVEN implementation, originally used for TxPoW's DECIMAL32 weight.
    if(numerator.isNegative()||denominator.isNegative()||denominator.isZero()||precision<1) throw std::invalid_argument("Invalid positive decimal division");
    if(numerator.isZero()) return MMREntryNumber(0);
    int exponent=int(numerator.toDecimalString().size())-int(denominator.toDecimalString().size());
    if(exponent>=0?numerator<scaleUp(denominator,exponent):scaleUp(numerator,-exponent)<denominator) --exponent;
    int scale=precision-1-exponent;
    auto a=scale>=0?scaleUp(numerator,scale):numerator;
    auto b=scale>=0?denominator:scaleUp(denominator,-int64_t(scale));
    auto quotient=a/b,remainder=a%b;
    if(remainder*BigInt(2)>b || (remainder*BigInt(2)==b && !(quotient%BigInt(2)).isZero())) quotient=quotient+BigInt(1);
    if(quotient.toDecimalString().size()>size_t(precision)) {quotient=quotient/BigInt(10);--scale;}
    if(remainder.isZero()) while(scale>0 && (quotient%BigInt(10)).isZero()) {quotient=quotient/BigInt(10);--scale;}
    return MMREntryNumber(quotient,scale);
}
MMREntryNumber MMREntryNumber::add(const MMREntryNumber& other) const {
    BigInt a,b;int32_t scale;align(*this,other,a,b,scale);return MMREntryNumber(a+b,scale);
}
MMREntryNumber MMREntryNumber::multiplyDecimal32(const BigInt& factor) const {
    auto value=mUnscaled*factor;auto magnitude=value.isNegative()?BigInt(0)-value:value;
    int64_t scale=mScale;
    while(magnitude.toDecimalString().size()>7) {
        auto discarded=int64_t(magnitude.toDecimalString().size())-7;
        auto divisor=scaleUp(BigInt(1),discarded);
        auto quotient=magnitude/divisor,remainder=magnitude%divisor;
        if(remainder*BigInt(2)>divisor || (remainder*BigInt(2)==divisor && !(quotient%BigInt(2)).isZero())) quotient=quotient+BigInt(1);
        magnitude=quotient;scale-=discarded;
    }
    if(scale<std::numeric_limits<int32_t>::min() || scale>std::numeric_limits<int32_t>::max()) throw std::overflow_error("Decimal scale overflow");
    return MMREntryNumber(value.isNegative()?BigInt(0)-magnitude:magnitude,int32_t(scale));
}
std::string MMREntryNumber::toBigDecimalString() const {
    return javaBigDecimalText(mUnscaled.toDecimalString(),mScale);
}
int MMREntryNumber::compareTo(const MMREntryNumber& other) const {BigInt a,b;int32_t scale;align(*this,other,a,b,scale);return a.compareTo(b);}
std::string MMREntryNumber::toString() const {
    if(mUnscaled.isZero()) return "0";
    auto value=mUnscaled.toDecimalString();bool negative=value.front()=='-';if(negative) value.erase(0,1);
    if(mScale<0) value.append(size_t(-int64_t(mScale)),'0');
    else if(mScale>0) {
        if(size_t(mScale)>=value.size()) value.insert(0,size_t(mScale)-value.size()+1,'0');
        value.insert(value.size()-mScale,1,'.');
        while(value.back()=='0') value.pop_back();
        if(value.back()=='.') value.pop_back();
    }
    return negative?"-"+value:value;
}
void MMREntryNumber::writeDataStream(std::vector<uint8_t>& output) const {wire::append(output,MiniNumber(mScale).serialise());wire::append(output,MiniData(mUnscaled.toJavaBytes()).serialise());}
std::vector<uint8_t> MMREntryNumber::serialise() const {return Streamable::serialise();}
void MMREntryNumber::readDataStream(std::span<const uint8_t> data,size_t& offset){auto value=deserialise(data,offset);mUnscaled=std::move(value.mUnscaled);mScale=value.mScale;}
MMREntryNumber MMREntryNumber::deserialise(std::span<const uint8_t> data,size_t& offset) {
    int scale=MiniNumber::deserialise(data,offset).getAsInt();auto raw=MiniData::deserialise(data,offset);
    if(raw.empty()) throw JavaNumberFormatException("Zero length BigInteger");
    return MMREntryNumber(BigInt::fromJavaBytes(raw.bytes()),scale);
}
}
