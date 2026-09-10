// All active assertions from the pinned DECTests: 4 constructor, 22 value,
// and 4 exact ExecutionException checks. Uses the actual function implementation.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/number/DEC.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
#include <variant>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("DECTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main(){
    DEC prototype;auto function=prototype.getNewFunction();
    require(function->getName()=="DEC");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("DEC");require(function->getName()=="DEC");require(function->getParameterNum()==0);
    auto contract=context();
    using Input=std::variant<int64_t,std::string>;
    const std::vector<std::pair<Input,std::string>> cases{
        {int64_t(-1),"-2"},
        {"-0.99999999999999999","-1.99999999999999999"},
        {"-0.99999","-1.99999"},
        {"-0.49999999999999999","-1.49999999999999999"},
        {"-0.49999","-1.49999"},
        {int64_t(0),"-1"},
        {"0.49999","-0.50001"},
        {"0.49999999999999999","-0.50000000000000001"},
        {"0.99999","-0.00001"},
        {"0.99999999999999999","-0.00000000000000001"},
        {int64_t(1),"0"}};
    for(const auto& [input,expected]:cases){
        function=prototype.getNewFunction();auto value=std::visit([](const auto& supplied){return std::make_shared<NumberValue>(supplied);},input);
        function->addParameter(std::make_shared<ConstantExpression>(value));
        auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);
        require(dynamic_cast<NumberValue&>(*result).toString()==expected);
    }
    auto invalidContract=context();
    function=prototype.getNewFunction();rejected([&]{function->runFunction(invalidContract);});
    const std::vector<ValuePtr> invalidValues{
        std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<StringValue>("Hello World")};
    for(const auto& value:invalidValues){function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(value));rejected([&]{function->runFunction(invalidContract);});}
}
