#pragma once
#include "Expression.hpp"
namespace minima::kissvm {
class VariableExpression : public Expression {
    std::string mVariableName;
public:
    explicit VariableExpression(std::string name):mVariableName(std::move(name)){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override {return "variable:"+mVariableName;}
};
}
