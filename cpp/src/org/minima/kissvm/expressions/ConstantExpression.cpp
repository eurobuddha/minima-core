#include "ConstantExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
ValuePtr ConstantExpression::getValue(Contract& contract) {contract.incrementInstructions();return mValue;}
}
