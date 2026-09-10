#include "MIN.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr MIN::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());ValuePtr result;
    for(const auto& expression:getAllParameters()) {auto value=expression->getValue(contract);checkIsOfType(*value,Value::VALUE_NUMBER);if(!result||static_cast<NumberValue&>(*value).getNumber().isLess(static_cast<NumberValue&>(*result).getNumber())) result=value;}return result;
}
}
