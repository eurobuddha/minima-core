#include "Value.hpp"
#include "BooleanValue.hpp"
#include "NumberValue.hpp"
#include "HexValue.hpp"
#include "StringValue.hpp"
#include "../tokens/ScriptTokenizer.hpp"
namespace minima::kissvm {
Value::~Value()=default;
void Value::verifyType(int type) const {if(getValueType()!=type) throw std::invalid_argument("Incorrect value type, expected "+getValueTypeString(type)+" found "+getValueTypeString(getValueType()));}
std::shared_ptr<Value> Value::getValue(const std::string& text) {
    if(text.starts_with("[")&&text.ends_with("]")) return std::make_shared<StringValue>(text.substr(1,text.size()-2));
    if(text.starts_with("0x")) return std::make_shared<HexValue>(text);
    if(text=="TRUE") {static auto value=std::shared_ptr<Value>(&BooleanValue::TRUE,[](Value*){});return value;}
    if(text=="FALSE") {static auto value=std::shared_ptr<Value>(&BooleanValue::FALSE,[](Value*){});return value;}
    if(text.starts_with("-")||ScriptTokenizer::isNumeric(text)) return std::make_shared<NumberValue>(text);
    throw std::invalid_argument("Invalid value : "+text);
}
int Value::getValueType(const std::string& text) {
    if(text.starts_with("[")&&text.ends_with("]")) return VALUE_SCRIPT;
    if(text.starts_with("0x")) return VALUE_HEX;
    if(text=="TRUE"||text=="FALSE") return VALUE_BOOLEAN;
    if(text.starts_with("-")||ScriptTokenizer::isNumeric(text)) return VALUE_NUMBER;
    throw std::invalid_argument("Invalid value type : "+text);
}
std::string Value::getValueTypeString(int type) {switch(type) {case VALUE_HEX:return "HEX";case VALUE_NUMBER:return "NUMBER";case VALUE_SCRIPT:return "SCRIPT";case VALUE_BOOLEAN:return "BOOLEAN";default:return "ERROR_UNKNOWN_TYPE";}}
void Value::checkSameType(const Value& a,const Value& b) {if(a.getValueType()!=b.getValueType()) throw std::invalid_argument("Operation requires that both value types MUST be the same");}
void Value::checkSameType(const Value& a,const Value& b,int type) {checkSameType(a,b);if(a.getValueType()!=type) throw std::invalid_argument("Operation requires that both value types are of type "+std::to_string(type));}
}
