// Thirty-three active assertions from the pinned Java source; production unchanged.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/hex/SUBSET.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("SUBSETTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static ValuePtr number(int value){return std::make_shared<NumberValue>(value);}
static ValuePtr hex(const std::string& value){return std::make_shared<HexValue>(value);}
static ValuePtr script(const std::string& value){return std::make_shared<StringValue>(value);}
static std::shared_ptr<MinimaFunction> prepared(SUBSET& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    SUBSET prototype;auto function=prototype.getNewFunction();require(function->getName()=="SUBSET");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("SUBSET");require(function->getName()=="SUBSET");require(function->getParameterNum()==0);
    auto contract=context();
    function=prepared(prototype,{number(0),number(2),hex("0x01234567")});auto result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="0x0123");
    function=prepared(prototype,{number(0),number(4),hex("0x01234567")});result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="0x01234567");
    const std::vector<std::vector<ValuePtr>> validMethodRejections{
        {number(12),number(13),hex("0x0123456789ABCDEF")},
        {number(0),number(12),hex("0x0123456789ABCDEF")},
        {number(0),number(5),script("Hello World Minima is here")},
        {number(13),number(19),script("Hello World Minima is here")},
        {number(22),number(32),hex("0x0123456789ABCDEF")},
        {number(0),number(12),hex("0x0123456789ABCDEF")}
    };
    for(const auto& values:validMethodRejections){function=prepared(prototype,values);rejected([&]{function->runFunction(contract);});}
    auto invalidContract=context();
    const std::vector<std::vector<ValuePtr>> invalid{
        {},{number(0)},{number(0),number(2)},
        {number(-9),number(-5),hex("0x01234567")},
        {number(2),number(0),hex("0x01234567")},
        {std::make_shared<BooleanValue>(true),number(2),hex("0x01234567")},
        {hex("0x00"),number(2),hex("0x01234567")},
        {script("Hello World"),number(2),hex("0x01234567")},
        {number(0),std::make_shared<BooleanValue>(true),hex("0x01234567")},
        {number(0),hex("0x00"),hex("0x01234567")},
        {number(0),script("Hello World"),hex("0x01234567")},
        {std::make_shared<BooleanValue>(true),number(5),script("Hello World Minima is here")},
        {hex("0x00"),number(5),script("Hello World Minima is here")},
        {script("Hello World"),number(5),script("Hello World Minima is here")},
        {number(0),std::make_shared<BooleanValue>(true),script("Hello World Minima is here")},
        {number(0),hex("0x00"),script("Hello World Minima is here")},
        {number(0),script("Hello World"),script("Hello World Minima is here")},
        {number(0),number(2),std::make_shared<BooleanValue>(true)},
        {number(0),number(2),number(3)}
    };
    for(const auto& values:invalid){function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});}
}
