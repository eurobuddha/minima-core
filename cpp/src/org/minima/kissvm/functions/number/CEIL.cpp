#include "CEIL.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr CEIL::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto number=contract.getNumberParam(0,*this);return std::make_shared<NumberValue>(number.getNumber().ceil());
}
}
