// All ten assertions from the pinned Java ScriptValueTests.testConstructors.
#include "org/minima/kissvm/values/StringValue.hpp"
#include <stdexcept>
using namespace minima::kissvm;
static void require(bool ok) {if(!ok)throw std::runtime_error("ScriptValueTests: should be equal");}
int main() {
    StringValue sv1(std::string{}),sv2(std::string("[]")),sv3(std::string("return true")),sv4(std::string("HELLO WORLD"));
    require(Value::getValue("[HELLO WORLD]")->toString()=="HELLO WORLD");
    require(sv2.toString()=="[]");
    require(sv3.toString()=="return true");
    require(sv4.toString()=="HELLO WORLD");
    require(sv3.add(sv4).toString()=="return trueHELLO WORLD");
    require(sv4.add(sv3).toString()=="HELLO WORLDreturn true");
    require(sv1.getValueType()==StringValue::VALUE_SCRIPT);
    require(sv2.getValueType()==StringValue::VALUE_SCRIPT);
    require(sv3.getValueType()==StringValue::VALUE_SCRIPT);
    require(sv4.getValueType()==StringValue::VALUE_SCRIPT);
}
