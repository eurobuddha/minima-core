// Preserves the active assertions of the pinned FLOORTests Java test.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/number/FLOOR.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static int assertions=0;
static void require(bool ok){++assertions;if(!ok)throw std::runtime_error("FLOORTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main(){
    FLOOR prototype;auto function=prototype.getNewFunction();
    require(function->getName()=="FLOOR");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("FLOOR");require(function->getName()=="FLOOR");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<ValuePtr,std::string>> cases{
        {std::make_shared<NumberValue>(-1),"-1"},
        {std::make_shared<NumberValue>("-0.99999999999999999"),"-1"},
        {std::make_shared<NumberValue>("-0.99999"),"-1"},
        {std::make_shared<NumberValue>("-0.49999999999999999"),"-1"},
        {std::make_shared<NumberValue>("-0.49999"),"-1"},
        {std::make_shared<NumberValue>(0),"0"},
        {std::make_shared<NumberValue>("0.49999"),"0"},
        {std::make_shared<NumberValue>("0.49999999999999999"),"0"},
        {std::make_shared<NumberValue>("0.99999"),"0"},
        {std::make_shared<NumberValue>("0.99999999999999999"),"0"},
        {std::make_shared<NumberValue>(1),"1"}};
    for(const auto& [input,expected]:cases){
        function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(input));
        auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);
        require(dynamic_cast<NumberValue&>(*result).toString()==expected);
    }
    auto invalidContract=context();
    function=prototype.getNewFunction();rejected([&]{function->runFunction(invalidContract);});
    const std::vector<ValuePtr> invalidValues{
        std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")};
    for(const auto& value:invalidValues){function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(value));rejected([&]{function->runFunction(invalidContract);});}
    if(assertions!=30)throw std::runtime_error("FLOORTests assertion count changed");
}
