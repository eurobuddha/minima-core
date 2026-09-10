#pragma once
#include "../StatementBlock.hpp"
namespace minima::kissvm {
class WHILEstatement : public Statement {
    ExpressionPtr mWhileCheck;
    std::shared_ptr<StatementBlock> mWhileBlock;
public:
    WHILEstatement(ExpressionPtr condition,std::shared_ptr<StatementBlock> action):mWhileCheck(std::move(condition)),mWhileBlock(std::move(action)){}
    void execute(Contract& contract) override;
    std::string toString() const override {return "WHILE "+mWhileCheck->toString();}
};
}
