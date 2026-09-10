#pragma once
#include "Expression.hpp"
#include "../values/BooleanValue.hpp"
#include "org/minima/objects/base/MiniData.hpp"
namespace minima::kissvm {
class OperatorExpression : public Expression {
    ExpressionPtr mLeft,mRight;
    int mType;
public:
    static constexpr int OPERATOR_ADD=0,OPERATOR_SUB=1,OPERATOR_MUL=2,OPERATOR_DIV=3,OPERATOR_NEG=4,OPERATOR_SHIFTL=5,OPERATOR_SHIFTR=6,OPERATOR_MODULO=7,OPERATOR_AND=8,OPERATOR_OR=9,OPERATOR_XOR=10,OPERATOR_NOT=11;
    OperatorExpression(ExpressionPtr left,int type);
    OperatorExpression(ExpressionPtr left,ExpressionPtr right,int type):mLeft(std::move(left)),mRight(std::move(right)),mType(type){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override;
    static MiniData andFastHEX(const MiniData& a,const MiniData& b);
    static MiniData orFastHEX(const MiniData& a,const MiniData& b,int type);
    static MiniData notFastHEX(const MiniData& a);
};
}
