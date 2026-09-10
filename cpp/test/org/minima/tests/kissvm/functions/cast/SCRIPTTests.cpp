// 23 active assertions in the pinned SCRIPTTests.java; cast implementation reused.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/cast/STRING.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "support/JavaExceptions.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("SCRIPTTests assertion");}
template<class E,class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const E&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static std::shared_ptr<MinimaFunction> prepared(STRING& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    STRING prototype;auto function=prototype.getNewFunction();require(function->getName()=="STRING");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("STRING");require(function->getName()=="STRING");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<ValuePtr,std::string>> cases{
        {std::make_shared<BooleanValue>(true),"TRUE"},
        {std::make_shared<BooleanValue>(false),"FALSE"},
        {std::make_shared<HexValue>("0x414243444546"),"0x414243444546"},
        {std::make_shared<HexValue>("0x4142434445464748494A4B4C4D4E4F505152535455565758595A"),"0x4142434445464748494A4B4C4D4E4F505152535455565758595A"},
        {std::make_shared<NumberValue>(0),"0"},
        {std::make_shared<NumberValue>(65535),"65535"},
        {std::make_shared<NumberValue>(-65535),"-65535"},
        {std::make_shared<StringValue>("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),"ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
        {std::make_shared<StringValue>("Hello World"),"Hello World"},
    };
    for(const auto& [input,expected]:cases){function=prepared(prototype,{input});auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_SCRIPT);require(dynamic_cast<StringValue&>(*result).toString()==expected);}
    auto invalidContract=context();function=prepared(prototype,{});rejected<ExecutionException>([&]{function->runFunction(invalidContract);});
}
