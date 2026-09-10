// 21 active assertions in the pinned BOOLTests.java; cast implementation reused.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/cast/BOOL.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "support/JavaExceptions.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("BOOLTests assertion");}
template<class E,class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const E&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static std::shared_ptr<MinimaFunction> prepared(BOOL& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    BOOL prototype;auto function=prototype.getNewFunction();require(function->getName()=="BOOL");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("BOOL");require(function->getName()=="BOOL");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<ValuePtr,std::string>> cases{
        {std::make_shared<BooleanValue>(true),"TRUE"},
        {std::make_shared<BooleanValue>(false),"FALSE"},
        {std::make_shared<HexValue>("0x01"),"TRUE"},
        {std::make_shared<HexValue>("0x00"),"FALSE"},
        {std::make_shared<NumberValue>(1),"TRUE"},
        {std::make_shared<NumberValue>(0),"FALSE"},
        {std::make_shared<StringValue>("Hello World"),"TRUE"},
        {std::make_shared<StringValue>("FALSE"),"FALSE"},
    };
    for(const auto& [input,expected]:cases){function=prepared(prototype,{input});auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_BOOLEAN);require(dynamic_cast<BooleanValue&>(*result).toString()==expected);}
    auto invalidContract=context();function=prepared(prototype,{});rejected<ExecutionException>([&]{function->runFunction(invalidContract);});
}
