#include "GlobalExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
ValuePtr GlobalExpression::getValue(Contract& contract) {contract.incrementInstructions();return contract.getGlobal(mGlobalType);}
}
