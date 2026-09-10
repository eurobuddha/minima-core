#pragma once
#include "../Statement.hpp"
namespace minima::kissvm {
class ASSERTstatement : public Statement {
    ExpressionPtr mValue;
public:
    explicit ASSERTstatement(ExpressionPtr value):mValue(std::move(value)){}
    void execute(Contract& contract) override;
    std::string toString() const override {return "ASSERT "+mValue->toString();}
};
}
