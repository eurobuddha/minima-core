#include "RETURNstatement.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
void RETURNstatement::execute(Contract& contract) {auto value=mValue->getValue(contract);if(value->getValueType()!=Value::VALUE_BOOLEAN) throw ExecutionException("RETURN MUST use a BOOLEAN expression : "+toString());bool success=static_cast<BooleanValue&>(*value).isTrue();contract.setRETURNValue(success);}
}
