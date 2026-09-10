#include "HexValue.hpp"
#include "support/KissLimits.hpp"
#include "support/JavaExceptions.hpp"
namespace minima::kissvm {
HexValue::HexValue(MiniData data):mData(std::move(data)) {if(mData.size()>limits::MAX_DATA_SIZE) throw std::invalid_argument("MAX HEX value size reached : "+std::to_string(mData.size())+"/"+std::to_string(limits::MAX_DATA_SIZE));}
HexValue::HexValue(const MiniNumber& number) {
    if(number.isLess(MiniNumber(0))) throw JavaNumberFormatException("HEXValue Number must be positive");
    if(!number.floor().isEqual(number)) throw JavaNumberFormatException("HEXValue Number must be a whole number");
    mData=MiniData(number.getAsBigInteger().toJavaBytes());
}
std::string HexValue::toString() const {return mData.to0xString();}
}
