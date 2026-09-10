#include "LETstatement.hpp"
#include "../../Contract.hpp"
namespace minima::kissvm {
namespace {std::string trim(std::string text) {size_t start=0,end=text.size();while(start<end&&uint8_t(text[start])<=32) ++start;while(end>start&&uint8_t(text[end-1])<=32) --end;return text.substr(start,end-start);}}
void LETstatement::execute(Contract& contract) {
    if(mLETType==LET_VARIABLE) {contract.setVariable(mName,mValue->getValue(contract));return;}
    std::string position;for(const auto& expression:mArrayPos) {auto value=expression->getValue(contract);if(value->getValueType()!=Value::VALUE_NUMBER) throw ExecutionException("Incorrect Parameter type for LET - MUST be a NumberValue @ "+trim(value->toString()));position+=trim(value->toString())+",";}
    contract.setVariable(position,mValue->getValue(contract));
}
std::string LETstatement::toString() const {if(mLETType==LET_VARIABLE) return "LET "+mName+" = "+mValue->toString();std::string result="LET ( ";for(const auto& expression:mArrayPos) result+=trim(expression->toString())+" ";return trim(result)+" ) = "+mValue->toString();}
}
