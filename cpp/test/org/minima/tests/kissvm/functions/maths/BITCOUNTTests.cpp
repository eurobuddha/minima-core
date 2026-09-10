// Pinned test: 34 successful assertion calls plus 256 table predicates.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/hex/BITCOUNT.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
#include <iostream>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("BITCOUNTTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main(int argc,char**){
    if(argc>1){
        BITCOUNT::BITSPERBYTE[1]=42;std::cout<<BITCOUNT::totalBits({1,1})<<'\n';
        BITCOUNT function;function.addParameter(std::make_shared<ConstantExpression>(std::make_shared<HexValue>("0x01")));
        auto contract=context();std::cout<<function.runFunction(contract)->toString()<<'\n';
        BITCOUNT::BITSPERBYTE[1]=INT32_MAX;BITCOUNT::BITSPERBYTE[2]=1;
        std::cout<<BITCOUNT::totalBits({1,2})<<'\n';return 0;
    }

    BITCOUNT prototype;auto function=prototype.getNewFunction();
    require(function->getName()=="BITCOUNT");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("BITCOUNT");require(function->getName()=="BITCOUNT");require(function->getParameterNum()==0);
    auto contract=context();
    std::array<int,256> expected{};
    for(int i=0;i<256;++i){int count=0,current=i;while(current>0){if(current%2==1)++count;current/=2;}expected[i]=count;}
    for(int i=0;i<256;++i)require(expected[i]==BITCOUNT::BITSPERBYTE[i]);
    const std::vector<std::pair<std::string,std::string>> cases{
        {"0x00","0"},{"0x01","1"},{"0x02","1"},{"0x04","1"},{"0x08","1"},
        {"0x0F","4"},{"0xF0","4"},{"0xFF","8"},{"0xFF0","8"},{"0xFF8","9"},
        {"0xFFF","12"},{"0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF","128"},{"0x1248","4"}};
    for(const auto& [input,expectedCount]:cases){
        function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<HexValue>(input)));
        auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);
        require(dynamic_cast<NumberValue&>(*result).toString()==expectedCount);
    }
    auto invalidContract=context();
    function=prototype.getNewFunction();rejected([&]{function->runFunction(invalidContract);});
    const std::vector<ValuePtr> invalidValues{
        std::make_shared<BooleanValue>(true),std::make_shared<NumberValue>(int64_t(123456798)),std::make_shared<StringValue>("Hello World")};
    for(const auto& value:invalidValues){function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(value));rejected([&]{function->runFunction(invalidContract);});}
}
