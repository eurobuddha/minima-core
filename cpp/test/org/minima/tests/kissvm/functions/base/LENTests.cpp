// Twelve active assertions in the pinned source; production function unchanged.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/hex/LEN.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("LENTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
static ValuePtr hex(const std::string& value){return std::make_shared<HexValue>(value);}
static std::shared_ptr<MinimaFunction> prepared(LEN& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    LEN prototype;auto function=prototype.getNewFunction();require(function->getName()=="LEN");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("LEN");require(function->getName()=="LEN");require(function->getParameterNum()==0);
    auto contract=context();function=prepared(prototype,{hex("0x01234567")});auto result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_NUMBER);require(dynamic_cast<NumberValue&>(*result).getNumber().getAsInt()==4);
    function=prepared(prototype,{std::make_shared<StringValue>("HEllo")});result=function->runFunction(contract);
    require(result->getValueType()==Value::VALUE_NUMBER);require(dynamic_cast<NumberValue&>(*result).getNumber().getAsInt()==5);
    auto invalidContract=context();
    const std::vector<std::vector<ValuePtr>> invalid{
        {},{hex("0x01234567"),hex("0x01234567")},
        {std::make_shared<BooleanValue>(true)},{std::make_shared<NumberValue>(100)}
    };
    for(const auto& values:invalid){function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});}
}
