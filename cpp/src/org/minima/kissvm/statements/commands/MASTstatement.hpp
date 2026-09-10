#pragma once
#include "../Statement.hpp"
namespace minima::kissvm {
class MASTstatement : public Statement {
    ExpressionPtr mValue;
public:
    explicit MASTstatement(ExpressionPtr value):mValue(std::move(value)){}
    void execute(Contract& contract) override;
    std::string toString() const override {return "MAST "+mValue->toString();}
};
}
