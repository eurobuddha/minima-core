#pragma once
#include "Expression.hpp"
namespace minima::kissvm {
class GlobalExpression : public Expression {
    std::string mGlobalType;
public:
    explicit GlobalExpression(std::string name):mGlobalType(std::move(name)){}
    ValuePtr getValue(Contract& contract) override;
    std::string toString() const override {return "global:"+mGlobalType;}
};
}
