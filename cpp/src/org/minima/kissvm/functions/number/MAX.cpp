#include "MAX.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr MAX::runFunction(Contract& contract) {
    checkMinParamNumber(requiredParams());ValuePtr result;
    for(const auto& expression:getAllParameters()) {auto value=expression->getValue(contract);checkIsOfType(*value,Value::VALUE_NUMBER);if(!result||static_cast<NumberValue&>(*value).getNumber().isMore(static_cast<NumberValue&>(*result).getNumber())) result=value;}return result;
}
}
