#include "FunctionExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
ValuePtr FunctionExpression::getValue(Contract& contract) {contract.incrementInstructions();contract.incrementStackDepth();auto value=mFunction->runFunction(contract);contract.decrementStackDepth();contract.traceLog(toString()+" returns:"+value->toString());return value;}
std::string FunctionExpression::toString() const {std::string result="function:"+mFunction->getName()+", params:[";bool first=true;for(const auto& param:mFunction->getAllParameters()) {if(!first) result+=", ";first=false;result+=param->toString();}return result+"]";}
}
