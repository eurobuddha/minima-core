#include "POW.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr POW::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto exponent=contract.getNumberParam(0,*this);auto number=contract.getNumberParam(1,*this);const auto& value=exponent.getNumber();
    if(!value.floor().isEqual(value)) throw ExecutionException("POW must be to a whole Number");
    if(value.abs().isMoreEqual(MiniNumber::THOUSAND24)) throw ExecutionException("ABS POW exponent must be less than 1024");
    return std::make_shared<NumberValue>(number.getNumber().pow(value.getAsInt()));
}
}
