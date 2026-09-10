#include "VariableExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
ValuePtr VariableExpression::getValue(Contract& contract) {contract.incrementInstructions();auto value=contract.getVariable(mVariableName);if(!value) throw ExecutionException("Variable does not exist : "+mVariableName);return value;}
}
