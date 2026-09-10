#include "ASSERTstatement.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
void ASSERTstatement::execute(Contract& contract) {auto value=mValue->getValue(contract);if(value->getValueType()!=Value::VALUE_BOOLEAN) throw ExecutionException("ASSERT MUST use a BOOLEAN expression : "+toString());bool success=static_cast<BooleanValue&>(*value).isTrue();if(!success) contract.setRETURNValue(false);}
}
