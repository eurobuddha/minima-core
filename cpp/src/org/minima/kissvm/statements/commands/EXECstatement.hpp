#pragma once
#include "../Statement.hpp"
namespace minima::kissvm {
class EXECstatement : public Statement {
    ExpressionPtr mValue;
public:
    explicit EXECstatement(ExpressionPtr value):mValue(std::move(value)){}
    void execute(Contract& contract) override;
    std::string toString() const override {return "EXEC "+mValue->toString();}
};
}
