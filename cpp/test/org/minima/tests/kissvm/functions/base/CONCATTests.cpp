// Twenty-one active assertions from the pinned Java source; production unchanged.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/hex/CONCAT.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("CONCATTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static ValuePtr hex(const std::string& value){return std::make_shared<HexValue>(value);}
static std::shared_ptr<MinimaFunction> prepared(CONCAT& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    CONCAT prototype;auto function=prototype.getNewFunction();require(function->getName()=="CONCAT");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("CONCAT");require(function->getName()=="CONCAT");require(function->getParameterNum()==0);
    auto contract=context();
    function=prepared(prototype,{hex("0x01234567"),hex("0x89ABCDEF")});auto result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="0x0123456789ABCDEF");
    function=prepared(prototype,{hex("0x01234567"),hex("0x89ABCDEF"),hex("0x01234567"),hex("0x89ABCDEF"),hex("0x01234567")});result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="0x0123456789ABCDEF0123456789ABCDEF01234567");
    auto invalidContract=context();
    for(const auto& values:std::vector<std::vector<ValuePtr>>{{},{hex("0x01234567")},{std::make_shared<StringValue>("HELLO")}}){
        function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});
    }
    function=prepared(prototype,{hex(""),hex("")});result=function->runFunction(invalidContract);
    require(result->getValueType()==Value::VALUE_HEX);require(dynamic_cast<HexValue&>(*result).toString()=="");
    const std::vector<std::vector<ValuePtr>> invalid{
        {hex("0x01234567"),std::make_shared<BooleanValue>(true)},
        {hex("0x01234567"),std::make_shared<NumberValue>(100)},
        {hex("0x01234567"),std::make_shared<StringValue>("HELLO")},
        {std::make_shared<StringValue>("HELLO"),std::make_shared<BooleanValue>(true)},
        {std::make_shared<StringValue>("HELLO"),hex("0x01234567")},
        {std::make_shared<StringValue>("HELLO"),std::make_shared<NumberValue>(100)},
        {std::make_shared<BooleanValue>(true),std::make_shared<BooleanValue>(true)},
        {std::make_shared<NumberValue>(100),std::make_shared<NumberValue>(100)}
    };
    for(const auto& values:invalid){function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});}
}
