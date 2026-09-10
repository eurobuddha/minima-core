// All 25 active assertions from the pinned POWTests.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/number/POW.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <stdexcept>
using namespace minima;using namespace minima::kissvm;
static void require(bool ok){if(!ok)throw std::runtime_error("POWTests assertion");}
template<class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const ExecutionException&){caught=true;}require(caught);}
static Contract context(){return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
int main(){
    POW prototype;auto fresh=prototype.getNewFunction();
    require(fresh->getName()=="POW");require(fresh->getParameterNum()==0);
    fresh=MinimaFunction::getFunction("POW");require(fresh->getName()=="POW");require(fresh->getParameterNum()==0);
    auto contract=context();
    auto function=prototype.getNewFunction();
    function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(int64_t(0))));
    function->addParameter(std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(int64_t(0))));
    auto result=function->runFunction(contract);require(result->getValueType()==Value::VALUE_NUMBER);
    require(dynamic_cast<NumberValue&>(*result).toString()=="1");
    auto invalidContract=context();
    auto rejectArguments=[&](const std::vector<ValuePtr>& values){
        auto function=prototype.getNewFunction();
        for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));
        rejected([&]{function->runFunction(invalidContract);});
    };
    for(int size:{0,1,3}){std::vector<ValuePtr> values;for(int i=0;i<size;++i)values.push_back(std::make_shared<NumberValue>(int64_t(0)));rejectArguments(values);}
    rejectArguments({std::make_shared<NumberValue>("0.5"),std::make_shared<NumberValue>(int64_t(16))});
    const std::vector<ValuePtr> values{std::make_shared<BooleanValue>(true),std::make_shared<HexValue>("0x01234567"),std::make_shared<NumberValue>(int64_t(0)),std::make_shared<StringValue>("Hello World")};
    for(size_t left=0;left<values.size();++left)for(size_t right=0;right<values.size();++right){
        if(left==2&&right==2)continue;
        rejectArguments({values[left],values[right]});
    }
}
