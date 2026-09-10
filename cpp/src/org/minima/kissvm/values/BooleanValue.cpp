#include "BooleanValue.hpp"
namespace minima::kissvm {
BooleanValue BooleanValue::TRUE(true),BooleanValue::FALSE(false);
std::string BooleanValue::toString() const {return mTrue?"TRUE":"FALSE";}
}
