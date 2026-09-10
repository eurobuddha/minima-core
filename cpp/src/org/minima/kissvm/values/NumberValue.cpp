#include "NumberValue.hpp"
namespace minima::kissvm {
std::string NumberValue::toString() const {return mNumber.toString();}
NumberValue NumberValue::add(const NumberValue& value) const {return NumberValue(mNumber.add(value.mNumber));}
NumberValue NumberValue::sub(const NumberValue& value) const {return NumberValue(mNumber.sub(value.mNumber));}
NumberValue NumberValue::mult(const NumberValue& value) const {return NumberValue(mNumber.mult(value.mNumber));}
NumberValue NumberValue::div(const NumberValue& value) const {return NumberValue(mNumber.div(value.mNumber));}
}
