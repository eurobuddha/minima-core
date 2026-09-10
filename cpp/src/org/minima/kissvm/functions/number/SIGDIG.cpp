#include "SIGDIG.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr SIGDIG::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto significant=contract.getNumberParam(0,*this);auto number=contract.getNumberParam(1,*this);const auto& value=significant.getNumber();
    if(!value.floor().isEqual(value)) throw ExecutionException("SIGDIG precision must be to a whole Number");
    if(value.isLess(MiniNumber::ZERO)) throw ExecutionException("SIGDIG precision must be a positive whole number : "+significant.toString());
    return std::make_shared<NumberValue>(number.getNumber().setSignificantDigits(value.getAsInt()));
}
}
