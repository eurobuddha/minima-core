#pragma once
// Reuse LENTests' direct function/constant-expression fixture and exact exception checks.
#include "org/minima/kissvm/Contract.hpp"
#include "org/minima/kissvm/functions/MinimaFunction.hpp"
#include "org/minima/kissvm/expressions/ConstantExpression.hpp"
#include <iostream>
#include <stdexcept>
namespace minima::original_function_tests {
using namespace kissvm;
inline int assertions=0;
inline void require(bool ok) {++assertions;if(!ok)throw std::runtime_error("Original function assertion "+std::to_string(assertions));}
inline Contract context() {return Contract("",std::string{},std::make_shared<Witness>(),std::make_shared<Transaction>());}
inline std::shared_ptr<MinimaFunction> prepared(const MinimaFunction& prototype,const std::vector<ValuePtr>& values) {
    auto function=prototype.getNewFunction();
    for(const auto& value:values)function->addParameter(std::make_shared<ConstantExpression>(value));
    return function;
}
inline void constructors(const MinimaFunction& prototype,const std::string& name) {
    auto function=prototype.getNewFunction();require(function->getName()==name);require(function->getParameterNum()==0);
    function=MinimaFunction::getFunction(name);require(function->getName()==name);require(function->getParameterNum()==0);
}
inline void rejected(const MinimaFunction& prototype,Contract& contract,const std::vector<ValuePtr>& values) {
    auto function=prepared(prototype,values);bool caught=false;
    try {function->runFunction(contract);}catch(const ExecutionException&) {caught=true;}
    require(caught);
}
inline void report(const std::string& name,int expected) {
    if(assertions!=expected)throw std::runtime_error("Executed assertion count changed");
    std::cout<<"ORIGINAL_SHA_RESULT {\"class\":\""<<name<<"\",\"tests\":3,\"assertions\":"<<assertions<<",\"passed\":true}\n";
}
}
