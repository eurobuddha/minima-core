#include "OperatorExpression.hpp"
#include "ConstantExpression.hpp"
#include "../Contract.hpp"
#include <algorithm>
namespace minima::kissvm {
OperatorExpression::OperatorExpression(ExpressionPtr left,int type):OperatorExpression(std::move(left),std::make_shared<ConstantExpression>(Value::getValue("FALSE")),type){}
ValuePtr OperatorExpression::getValue(Contract& contract) {
    contract.incrementInstructions();auto left=mLeft->getValue(contract);auto right=mRight->getValue(contract);ValuePtr result;
    switch(mType) {
        case OPERATOR_ADD:
            Value::checkSameType(*left,*right);
            if(left->getValueType()==Value::VALUE_NUMBER) return std::make_shared<NumberValue>(static_cast<NumberValue&>(*left).add(static_cast<NumberValue&>(*right)));
            if(left->getValueType()==Value::VALUE_SCRIPT) return std::make_shared<StringValue>(static_cast<StringValue&>(*left).add(static_cast<StringValue&>(*right)));
            throw ExecutionException("Invalid type in ADD. MUST be Number or String "+left->toString());
        case OPERATOR_SUB:case OPERATOR_MUL:case OPERATOR_DIV:case OPERATOR_MODULO: {
            Value::checkSameType(*left,*right,Value::VALUE_NUMBER);const auto& l=static_cast<NumberValue&>(*left).getNumber();const auto& r=static_cast<NumberValue&>(*right).getNumber();
            if(mType==OPERATOR_DIV&&r.isEqual(MiniNumber::ZERO)) throw ExecutionException("Divide By ZERO! "+toString());
            result=std::make_shared<NumberValue>(mType==OPERATOR_SUB?l.sub(r):mType==OPERATOR_MUL?l.mult(r):mType==OPERATOR_DIV?l.div(r):l.modulo(r));break;
        }
        case OPERATOR_NEG:left->verifyType(Value::VALUE_NUMBER);result=std::make_shared<NumberValue>(static_cast<NumberValue&>(*left).getNumber().mult(MiniNumber::MINUSONE));break;
        case OPERATOR_SHIFTL:case OPERATOR_SHIFTR: {
            left->verifyType(Value::VALUE_HEX);right->verifyType(Value::VALUE_NUMBER);const auto& h=static_cast<HexValue&>(*left).getMiniData();const auto& n=static_cast<NumberValue&>(*right).getNumber();
            if(n.abs().isMore(Contract::MAX_BITSHIFT)) throw ExecutionException(std::string("Can only SHIFT")+(mType==OPERATOR_SHIFTL?"LEFT ":"RIGHT ")+Contract::MAX_BITSHIFT.toString()+" bits MAX "+n.toString());
            result=std::make_shared<HexValue>(mType==OPERATOR_SHIFTL?h.shiftl(n.getAsInt()):h.shiftr(n.getAsInt()));break;
        }
        case OPERATOR_AND:case OPERATOR_OR:case OPERATOR_XOR: {
            Value::checkSameType(*left,*right,Value::VALUE_HEX);const auto& l=static_cast<HexValue&>(*left).getMiniData();const auto& r=static_cast<HexValue&>(*right).getMiniData();
            result=std::make_shared<HexValue>(mType==OPERATOR_AND?andFastHEX(l,r):orFastHEX(l,r,mType==OPERATOR_OR?0:1));break;
        }
        case OPERATOR_NOT:left->verifyType(Value::VALUE_HEX);result=std::make_shared<HexValue>(notFastHEX(static_cast<HexValue&>(*left).getMiniData()));break;
        default:throw ExecutionException("UNKNOWN operator");
    }
    contract.traceLog(toString()+" returns:"+result->toString());return result;
}
std::string OperatorExpression::toString() const {
    if(mType==OPERATOR_NEG) return " - ( "+mLeft->toString()+" )";
    if(mType==OPERATOR_NOT) return " ~ ( "+mLeft->toString()+" )";
    static const char* names[]={"+","-","*","/","ERROR","<<",">>","%","&","|","^"};
    return "( "+mLeft->toString()+" "+(mType>=0&&mType<11?names[mType]:"ERROR")+" "+mRight->toString()+" )";
}
namespace {
MiniData stripZeros(std::vector<uint8_t> bytes) {auto start=std::find_if(bytes.begin(),bytes.end(),[](uint8_t b){return b!=0;});if(start==bytes.end()) return MiniData(std::vector<uint8_t>{0});bytes.erase(bytes.begin(),start);return MiniData(std::move(bytes));}
}
MiniData OperatorExpression::andFastHEX(const MiniData& a,const MiniData& b) {
    size_t length=std::min(a.size(),b.size());std::vector<uint8_t> result(length);
    for(size_t i=0;i<length;++i) result[i]=a.bytes()[a.size()-length+i]&b.bytes()[b.size()-length+i];return stripZeros(std::move(result));
}
MiniData OperatorExpression::orFastHEX(const MiniData& a,const MiniData& b,int type) {
    size_t length=std::max(a.size(),b.size());std::vector<uint8_t> result(length);
    for(size_t i=0;i<length;++i) {uint8_t l=i<a.size()?a.bytes()[a.size()-1-i]:0,r=i<b.size()?b.bytes()[b.size()-1-i]:0;result[length-1-i]=type==0?l|r:l^r;}return stripZeros(std::move(result));
}
MiniData OperatorExpression::notFastHEX(const MiniData& a) {auto result=a.bytes();for(auto& b:result) b=uint8_t(~b);return stripZeros(std::move(result));}
}
