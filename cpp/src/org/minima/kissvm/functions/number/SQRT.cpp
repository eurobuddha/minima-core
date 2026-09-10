#include "SQRT.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr SQRT::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto number=contract.getNumberParam(0,*this);return std::make_shared<NumberValue>(number.getNumber().sqrt());
}
}
