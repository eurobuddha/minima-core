#include "INC.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
ValuePtr INC::runFunction(Contract& contract) {
    checkExactParamNumber(requiredParams());auto number=contract.getNumberParam(0,*this);return std::make_shared<NumberValue>(number.getNumber().increment());
}
}
