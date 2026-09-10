#include "DUMMYFUNCTION.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include "org/minima/kissvm/values/BooleanValue.hpp"
#include "org/minima/kissvm/values/NumberValue.hpp"
#include "org/minima/kissvm/exceptions/ExecutionException.hpp"
#include "org/minima/kissvm/exceptions/MinimaParseException.hpp"
#include <stdexcept>
using namespace minima::kissvm;using namespace minima::kissvm::tests;
static void require(bool ok){if(!ok)throw std::runtime_error("MinimaFunctionTests assertion");}
template<class E,class F>static void rejected(F operation){bool caught=false;try{operation();}catch(const E&){caught=true;}require(caught);}
int main(){
    auto function=MinimaFunction::getFunction("CONCAT");require(function->getName()=="CONCAT");
    rejected<MinimaParseException>([]{MinimaFunction::getFunction("DUMMYFUNCTION");});
    DUMMYFUNCTION first;function=first.getNewFunction();
    require(function->getName()=="DUMMYFUNCTION");require(function->getParameterNum()==0);
    DUMMYFUNCTION second;function=second.getNewFunction();
    require(function->getName()=="DUMMYFUNCTION");require(function->getParameterNum()==0);
    auto a=std::make_shared<ConstantExpression>(std::make_shared<BooleanValue>(true));
    function->addParameter(a);require(function->getParameterNum()==1);require(function->getParameter(0)==a);
    auto b=std::make_shared<ConstantExpression>(std::make_shared<NumberValue>(1));
    function->addParameter(b);require(function->getParameter(1)==b);
    rejected<ExecutionException>([&]{function->getParameter(2);});require(function->getParameterNum()==2);
}
