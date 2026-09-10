// Adapted from the inspected Java ValueTests getters and type checks.
#include "org/minima/kissvm/values/Value.hpp"
#include "org/minima/kissvm/values/BooleanValue.hpp"
#include <array>
#include <stdexcept>
using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("ValueTests assertion");}
template<class F> static void rejected(F fn) {bool threw=false;try {fn();}catch(const std::invalid_argument&) {threw=true;}require(threw);}
int main() {
    require(Value::getValue("[ RETURN TRUE ]")->toString()==" RETURN TRUE ");
    require(Value::getValue("0xFFFF")->toString()=="0xFFFF");
    require(Value::getValue("5")->toString()=="5");
    require(Value::getValue("TRUE").get()==&BooleanValue::TRUE);
    require(Value::getValue("FALSE").get()==&BooleanValue::FALSE);
    for(const auto* value:{"!]","[!","!","z"}) {rejected([&]{Value::getValue(value);});rejected([&]{Value::getValueType(value);});}
    std::array<std::string,4> inputs{"TRUE","0x00","0","[Hello World]"};
    std::array<int,4> types{8,1,2,4};std::array<std::string,4> names{"BOOLEAN","HEX","NUMBER","SCRIPT"};
    for(size_t i=0;i<inputs.size();++i) {
        auto value=Value::getValue(inputs[i]);require(Value::getValueType(inputs[i])==types[i]);require(Value::getValueTypeString(types[i])==names[i]);
        value->verifyType(types[i]);Value::checkSameType(*value,*value);Value::checkSameType(*value,*value,types[i]);
        for(size_t j=0;j<inputs.size();++j) if(i!=j) {
            rejected([&]{value->verifyType(types[j]);});
            rejected([&]{Value::checkSameType(*value,*Value::getValue(inputs[j]));});
            rejected([&]{Value::checkSameType(*value,*Value::getValue(inputs[j]),types[i]);});
            rejected([&]{Value::checkSameType(*value,*value,types[j]);});
        }
    }
    require(Value::getValueTypeString(-99)=="ERROR_UNKNOWN_TYPE");
}
