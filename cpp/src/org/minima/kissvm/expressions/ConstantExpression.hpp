#pragma once
#include "Expression.hpp"
namespace minima::kissvm {
class ConstantExpression : public Expression {
    ValuePtr mValue;
public:
    explicit ConstantExpression(ValuePtr value):mValue(std::move(value)){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override {return mValue->toString();}
};
}
