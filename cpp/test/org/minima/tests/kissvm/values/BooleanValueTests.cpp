// Adapted from Java BooleanValueTests.
#include "org/minima/kissvm/values/BooleanValue.hpp"
#include <stdexcept>
using namespace minima::kissvm;
static void require(bool ok) {if(!ok) throw std::runtime_error("BooleanValueTests assertion");}
int main() {
    BooleanValue yes(true),no(false);
    for(auto* value:{&yes,&BooleanValue::TRUE}) {require(value->getValueType()==8);require(value->toString()=="TRUE");require(value->isTrue()&&!value->isFalse());require(value->isEqual(yes)&&!value->isEqual(no));}
    for(auto* value:{&no,&BooleanValue::FALSE}) {require(value->getValueType()==8);require(value->toString()=="FALSE");require(value->isFalse()&&!value->isTrue());require(value->isEqual(no)&&!value->isEqual(yes));}
}
