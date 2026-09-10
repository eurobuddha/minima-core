#include "BooleanExpression.hpp"
#include "ConstantExpression.hpp"
#include "../Contract.hpp"
namespace minima::kissvm {
BooleanExpression::BooleanExpression(ExpressionPtr left,int type):BooleanExpression(std::move(left),std::make_shared<ConstantExpression>(Value::getValue("FALSE")),type){}
std::shared_ptr<BooleanValue> BooleanExpression::getBooleanValue(Contract& contract) {return std::dynamic_pointer_cast<BooleanValue>(getValue(contract));}
ValuePtr BooleanExpression::getValue(Contract& contract) {
    contract.incrementInstructions();auto left=mLeft->getValue(contract);auto right=mRight->getValue(contract);
    if(mType!=BOOLEAN_NOT) Value::checkSameType(*left,*right);
    bool result=false;
    if(mType<=BOOLEAN_NXOR&&mType>=BOOLEAN_AND) {
        left->verifyType(Value::VALUE_BOOLEAN);bool l=static_cast<BooleanValue&>(*left).isTrue(),r=static_cast<BooleanValue&>(*right).isTrue();
        switch(mType) {case BOOLEAN_AND:result=l&&r;break;case BOOLEAN_NAND:result=!(l&&r);break;case BOOLEAN_OR:result=l||r;break;case BOOLEAN_NOR:result=!(l||r);break;case BOOLEAN_XOR:result=l!=r;break;case BOOLEAN_NXOR:result=l==r;break;}
    } else if(mType==BOOLEAN_NOT) {left->verifyType(Value::VALUE_BOOLEAN);result=static_cast<BooleanValue&>(*left).isFalse();}
    else if(mType==BOOLEAN_EQ||mType==BOOLEAN_NEQ) {
        switch(left->getValueType()) {
            case Value::VALUE_BOOLEAN:result=static_cast<BooleanValue&>(*left).isEqual(static_cast<BooleanValue&>(*right));break;
            case Value::VALUE_NUMBER:result=static_cast<NumberValue&>(*left).isEqual(static_cast<NumberValue&>(*right));break;
            case Value::VALUE_HEX:result=static_cast<HexValue&>(*left).isEqual(static_cast<HexValue&>(*right));break;
            case Value::VALUE_SCRIPT:result=static_cast<StringValue&>(*left).isEqual(static_cast<StringValue&>(*right));break;
        }
        if(mType==BOOLEAN_NEQ) result=!result;
    } else if(mType>=BOOLEAN_LT&&mType<=BOOLEAN_GTE) {
        left->verifyType(Value::VALUE_NUMBER);const auto& l=static_cast<NumberValue&>(*left);const auto& r=static_cast<NumberValue&>(*right);
        switch(mType) {case BOOLEAN_LT:result=l.isLess(r);break;case BOOLEAN_LTE:result=l.isLessEqual(r);break;case BOOLEAN_GT:result=l.isMore(r);break;case BOOLEAN_GTE:result=l.isMoreEqual(r);break;}
    } else throw ExecutionException("UNKNOWN boolean operator : "+std::to_string(mType));
    auto value=Value::getValue(result?"TRUE":"FALSE");contract.traceLog(toString()+" returns:"+value->toString());return value;
}
std::string BooleanExpression::toString() const {
    if(mType==BOOLEAN_NOT) return "NOT ( "+mLeft->toString()+" )";
    static const char* names[]={"AND","NAND","OR","NOR","XOR","NXOR","EQ","NEQ","LT","LTE","GT","GTE"};
    return "( "+mLeft->toString()+" "+(mType>=0&&mType<12?names[mType]:"ERROR")+" "+mRight->toString()+" )";
}
}
