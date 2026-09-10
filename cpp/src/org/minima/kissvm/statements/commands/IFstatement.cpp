#include "IFstatement.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
void IFstatement::execute(Contract& contract) {for(size_t i=0;i<mConditions.size();++i) {auto value=mConditions[i]->getValue(contract);if(value->getValueType()!=Value::VALUE_BOOLEAN) throw ExecutionException("IF conditional MUST use a BOOLEAN expression : "+mConditions[i]->toString());if(static_cast<BooleanValue&>(*value).isTrue()) {mActions[i]->run(contract);break;}}}
std::string IFstatement::toString() const {std::string result;for(size_t i=0;i<mConditions.size();++i) result+=(i?", ELSEIF ":"IF ")+mConditions[i]->toString();return result;}
}
