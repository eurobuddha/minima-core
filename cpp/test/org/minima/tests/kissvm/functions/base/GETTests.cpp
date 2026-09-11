// Thirty-one active assertions from the pinned Java source; production unchanged.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/general/GET.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("GETTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static ValuePtr number(int value){return std::make_shared<NumberValue>(value);}
static Contract context(bool mixed){
    Contract contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());
    const std::vector<std::string> keys{"0,","1,","0,0,","0,1,","1,0,","1,1,","0,0,0,0,0,","0,0,0,0,1,","0,0,0,41,0,","0,0,32,0,0,","5,5,5,5,5,"};
    for(size_t i=0;i<keys.size();++i)contract.setVariable(keys[i],number(int(i)+1));
    if(mixed)contract.setVariable("TRUE,0x1234,-99,hello world,LET a = 5,",number(12));
    return contract;
}
static std::shared_ptr<MinimaFunction> prepared(GET& prototype,const std::vector<ValuePtr>& values){auto function=prototype.getNewFunction();for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));return function;}
int main(){
    GET prototype;auto function=prototype.getNewFunction();require(function->getName()=="GET");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("GET");require(function->getName()=="GET");require(function->getParameterNum()==0);
    auto contract=context(true);
    const std::vector<std::vector<int>> indices{{0},{1},{0,0},{0,1},{1,0},{1,1},{0,0,0,0,0},{0,0,0,0,1},{0,0,0,41,0},{0,0,32,0,0},{5,5,5,5,5}};
    for(size_t i=0;i<indices.size();++i){
        std::vector<ValuePtr> values;for(int index:indices[i])values.push_back(number(index));
        function=prepared(prototype,values);auto result=function->runFunction(contract);
        require(result->getValueType()==Value::VALUE_NUMBER);require(dynamic_cast<NumberValue&>(*result).getNumber().getAsInt()==int(i)+1);
    }
    auto invalidContract=context(false);
    const std::vector<std::vector<ValuePtr>> invalid{{},{std::make_shared<BooleanValue>(true)},{std::make_shared<HexValue>("0x1234")},{std::make_shared<StringValue>("HELLO WORLD")},{number(1234567890)}};
    for(const auto& values:invalid){function=prepared(prototype,values);rejected([&]{function->runFunction(invalidContract);});}
}
