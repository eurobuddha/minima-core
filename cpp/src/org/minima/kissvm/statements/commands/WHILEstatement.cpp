#include "WHILEstatement.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
void WHILEstatement::execute(Contract& contract) {
    for(;;) {auto value=mWhileCheck->getValue(contract);if(value->getValueType()!=Value::VALUE_BOOLEAN) throw ExecutionException("RETURN MUST use a BOOLEAN expression : "+toString());if(static_cast<BooleanValue&>(*value).isFalse()) return;mWhileBlock->run(contract);if(contract.isSuccessSet()) return;}
}
}
