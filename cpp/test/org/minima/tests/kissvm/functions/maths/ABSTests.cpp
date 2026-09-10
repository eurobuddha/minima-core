// All active assertions from the pinned ABSTests: 4 constructor, 14 value,
// and 5 exact ExecutionException checks. Uses the actual function implementation.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/number/ABS.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <limits>
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("ABSTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main(){
    ABS prototype;auto function=prototype.getNewFunction();
    require(function->getName()=="ABS");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("ABS");require(function->getName()=="ABS");require(function->getParameterNum()==0);
    auto contract=context();
    const std::vector<std::pair<int64_t,std::string>> cases{
        {0,"0"},{-1,"1"},{1,"1"},{-32768,"32768"},{32767,"32767"},
        {std::numeric_limits<int64_t>::min(),"9223372036854775808"},
        {std::numeric_limits<int64_t>::max(),"9223372036854775807"}};
    for(const auto& [input,expected]:cases){
        function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(input)));
        auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);
        require(dynamic_cast<NumberValue&>(*result).toString()==expected);
    }
    auto invalidContract=context();
    function=prototype.getNewFunction();rejected([&]{function->runFunction(invalidContract);});
    function=prototype.getNewFunction();
    function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));
    function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(0)));
    rejected([&]{function->runFunction(invalidContract);});
    const std::vector<ValuePtr> invalidValues{
        std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")};
    for(const auto& value:invalidValues){function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(value));rejected([&]{function->runFunction(invalidContract);});}
}
