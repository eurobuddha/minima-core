#pragma once
#include "Expression.hpp"
#include "../values/BooleanValue.hpp"
#include "org/minima/objects/base/MiniData.hpp"
namespace minima::kissvm {
class BooleanExpression : public Expression {
    ExpressionPtr mLeft,mRight;
    int mType;
public:
    static constexpr int BOOLEAN_AND=0,BOOLEAN_NAND=1,BOOLEAN_OR=2,BOOLEAN_NOR=3,BOOLEAN_XOR=4,BOOLEAN_NXOR=5,BOOLEAN_EQ=6,BOOLEAN_NEQ=7,BOOLEAN_LT=8,BOOLEAN_LTE=9,BOOLEAN_GT=10,BOOLEAN_GTE=11,BOOLEAN_NOT=12;
    BooleanExpression(ExpressionPtr left,int type);
    BooleanExpression(ExpressionPtr left,ExpressionPtr right,int type):mLeft(std::move(left)),mRight(std::move(right)),mType(type){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override;
    std::shared_ptr<BooleanValue> getBooleanValue(Contract& contract);
};
}
