// Active cases from pinned HexValueTests; commented ordering tests stay inactive.
#include "org/minima/kissvm/values/HexValue.hpp"
#include "support/JavaExceptions.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("HexValueTests assertion");}
template<class F>static void formatError(F operation){bool caught=false;try{operation();}catch(const JavaNumberFormatException&){caught=true;}require(caught);}
static void constructors(){
    HexValue hv1(MiniData{}),hv2(MiniData::fromHex("1234"));
    formatError([]{HexValue value(MiniData::fromHex("QWER"));});
    formatError([]{HexValue value(MiniNumber(-1));});
    formatError([]{HexValue value(MiniNumber("1.1"));});
    HexValue hv4(MiniData(std::vector<uint8_t>{})),hv5(MiniData(std::vector<uint8_t>{0x12,0x34,0x56}));
    HexValue hv6(MiniNumber(int64_t(255))),hv7(MiniNumber(int64_t(65535)));
    HexValue hv8(MiniNumber(BigInt(int64_t(16777215)),0)),hv9(MiniNumber("1.0"));
    for(const auto* value:{&hv1,&hv2,&hv4,&hv5,&hv6,&hv7,&hv8,&hv9})require(value->getValueType()==HexValue::VALUE_HEX);
}
static HexValue zero(int route){
    switch(route){
    case 1:return HexValue(MiniData::fromHex("0"));
    case 2:return HexValue(std::vector<uint8_t>{0});
    case 3:return HexValue(MiniNumber(int64_t(0)));
    case 4:return HexValue(MiniNumber(int64_t(0)));
    case 5:return HexValue(MiniNumber(BigInt(int64_t(0)),0));
    case 6:return HexValue(std::string("0"));
    default:throw std::runtime_error("invalid fixture route");
    }
}
static HexValue unequal(int route,bool second){
    switch(route){
    case 1:return HexValue(MiniData::fromHex(second?"10":"5")); // Source literal is hex10, not decimal10.
    case 2:return HexValue(std::vector<uint8_t>{uint8_t(second?0x0A:0x05)});
    case 3:return HexValue(MiniNumber(int64_t(second?10:5)));
    case 4:return HexValue(MiniNumber(int64_t(second?10:5)));
    case 5:return HexValue(MiniNumber(BigInt(int64_t(second?10:5)),0));
    case 6:return HexValue(std::string(second?"A":"5"));
    default:throw std::runtime_error("invalid fixture route");
    }
}
int main(){
    constructors();
    for(int route=1;route<=6;++route){auto a=zero(route),b=zero(route);require(a.isEqual(b));require(b.isEqual(a));}
    for(int route=1;route<=6;++route){auto a=unequal(route,false),b=unequal(route,true);require(!a.isEqual(b));require(!a.isEqual(b));} // Preserve the source's repeated direction.
}
