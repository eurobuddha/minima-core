// 24 active assertions in the pinned HEXTests.java; cast implementation reused.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/cast/HEX.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "support/JavaExceptions.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("HEXTests assertion");}
template<class E,class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const E&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static std::shared_ptr<MinimaFunction> prepared(HEX& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    HEX prototype;auto function=prototype.getNewFunction();require(function->getName()=="HEX");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("HEX");require(function->getName()=="HEX");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<ValuePtr,std::string>> cases{
        {std::make_shared<BooleanValue>(true),"0x01"},
        {std::make_shared<BooleanValue>(false),"0x00"},
        {std::make_shared<HexValue>("0x01234567"),"0x01234567"},
        {std::make_shared<HexValue>("0x0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"),"0x0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"},
        {std::make_shared<NumberValue>(1),"0x01"},
        {std::make_shared<NumberValue>(0),"0x00"},
        {std::make_shared<NumberValue>(65535),"0xFFFF"},
        {std::make_shared<StringValue>("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),"0x4142434445464748494A4B4C4D4E4F505152535455565758595A"},
        {std::make_shared<StringValue>("Hello World"),"0x48656C6C6F20576F726C64"},
    };
    for(const auto& [input,expected]:cases){function=prepared(prototype,{input});auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()==expected);}
    auto invalidContract=context();function=prepared(prototype,{});rejected<ExecutionException>([&]{function->runFunction(invalidContract);});
    function=prepared(prototype,{std::make_shared<NumberValue>(0),std::make_shared<NumberValue>(0)});rejected<ExecutionException>([&]{function->runFunction(invalidContract);});
}
