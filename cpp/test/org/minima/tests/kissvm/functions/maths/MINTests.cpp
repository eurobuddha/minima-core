// Active pinned MINtests, with reproducible native background samples.
// Java's unseeded Random inputs are not claimed to be identical to these samples.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/number/MIN.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "support/JavaRandom48.hpp"
#include <bit>
#include <limits>
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static int assertions=0;
static void require(bool ok){++assertions;if(!ok)throw std::runtime_error("MINTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
// Reuse bounded Random draws to assemble signed samples without signed shifts or overflow.
static int64_t sample(JavaRandom48& random,bool wide){
    uint64_t bits=0;for(int part=0;part<(wide?4:2);++part)bits=(bits<<16)|uint64_t(random.nextInt(65536));
    return wide?std::bit_cast<int64_t>(bits):int64_t(std::bit_cast<int32_t>(uint32_t(bits)));
}
int main(){
    MIN prototype;auto function=prototype.getNewFunction();
    require(function->getName()=="MIN");require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction("MIN");require(function->getName()=="MIN");require(function->getParameterNum()==0);
    auto contract=context();contract.setMaxInstructions(std::numeric_limits<int32_t>::max());
    auto addNumber=[&](int64_t value){function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(value)));};
    auto checkNumber=[&](int64_t expected){auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);require(dynamic_cast<NumberValue&>(*result).toString()==std::to_string(expected));};
    function=prototype.getNewFunction();addNumber(0);addNumber(0);checkNumber(0);
    // Source order: int/long boundary first, last after 12, then middle between 12+12.
    for(int group=0;group<6;++group){
        bool wide=group%2!=0;int before=group<2?0:12,after=group<2?1:(group<4?0:12);
        int64_t boundary=wide?std::numeric_limits<int64_t>::min():int64_t(std::numeric_limits<int32_t>::min());
        for(int trial=0;trial<10000;++trial){
            JavaRandom48 random(0x4d494e4d4158ULL+uint64_t(group)*10000+uint64_t(trial));
            function=prototype.getNewFunction();
            for(int j=0;j<before;++j)addNumber(sample(random,wide));
            addNumber(boundary);
            for(int j=0;j<after;++j)addNumber(sample(random,wide));
            checkNumber(boundary);
        }
    }
    auto invalidContract=context();
    function=prototype.getNewFunction();rejected([&]{function->runFunction(invalidContract);});
    function=prototype.getNewFunction();addNumber(0);rejected([&]{function->runFunction(invalidContract);});
    // Source ordered pairs: Boolean, Hex, Number, String; only Number/Number is valid.
    auto value=[](int type)->ValuePtr{
        if(type==0)return std::make_shared<BooleanValue>(true);
        if(type==1)return std::make_shared<HexValue>("0x01234567");
        if(type==2)return std::make_shared<NumberValue>(0);
        return std::make_shared<StringValue>("Hello World");
    };
    for(int first=0;first<4;++first)for(int second=0;second<4;++second){
        if(first==2&&second==2)continue;
        function=prototype.getNewFunction();function->addParameter(std::make_shared<ConstantExpression>(value(first)));function->addParameter(std::make_shared<ConstantExpression>(value(second)));
        rejected([&]{function->runFunction(invalidContract);});
    }
    if(assertions!=120023)throw std::runtime_error("MINTests assertion count changed");
}
