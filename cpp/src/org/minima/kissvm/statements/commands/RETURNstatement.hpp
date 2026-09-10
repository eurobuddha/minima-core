#pragma once
#include "../Statement.hpp"
namespace minima::kissvm {
class RETURNstatement : public Statement {
    ExpressionPtr mValue;
public:
    explicit RETURNstatement(ExpressionPtr value):mValue(std::move(value)){}
    void execute(Contract& contract) override;
    std::string toString() const override {return "RETURN "+mValue->toString();}
};
}
