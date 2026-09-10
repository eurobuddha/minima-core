// 19 active assertions in the pinned NUMBERTests.java; cast implementation reused.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/cast/NUMBER.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "support/JavaExceptions.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("NUMBERTests assertion");}
template<class E,class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const E&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static std::shared_ptr<MinimaFunction> prepared(NUMBER& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    NUMBER prototype;auto function=prototype.getNewFunction();require(function->getName()=="NUMBER");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("NUMBER");require(function->getName()=="NUMBER");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<ValuePtr,std::string>> cases{
        {std::make_shared<BooleanValue>(true),"1"},
        {std::make_shared<BooleanValue>(false),"0"},
        {std::make_shared<HexValue>("0xFFFF"),"65535"},
        {std::make_shared<HexValue>("0xFFFFFFFFFFFFFFFF"),"18446744073709551615"},
        {std::make_shared<NumberValue>(-65536),"-65536"},
        {std::make_shared<NumberValue>(65535),"65535"},
    };
    for(const auto& [input,expected]:cases){function=prepared(prototype,{input});auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_NUMBER);require(dynamic_cast<NumberValue&>(*result).toString()==expected);}
    function=prepared(prototype,{std::make_shared<StringValue>("ABCDEFGHIJKLMNOPQRSTUVWXYZ")});rejected<JavaNumberFormatException>([&]{function->runFunction(contract);});
    function=prepared(prototype,{std::make_shared<StringValue>("Hello World")});rejected<JavaNumberFormatException>([&]{function->runFunction(contract);});
    auto invalidContract=context();function=prepared(prototype,{});rejected<ExecutionException>([&]{function->runFunction(invalidContract);});
}
